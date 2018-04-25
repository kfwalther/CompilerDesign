/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "ParseTreeVisitorImpl.h"
#include "ErrorHandler.h"
#include "Compiler.h"
#include "AstNode.h"
#include "SymbolTable.h"
#include "SymbolTableManager.h"
#include "Scope.h"
#include "SemanticAnalyzer.h"

/** Define the default constructor for the Parse Tree Listener. */
ParseTreeVisitorImpl::ParseTreeVisitorImpl(Compiler * const compiler) : compiler(compiler) {
	return;
}

/** Define a helper function to update the parent attribute of each child node. */
void ParseTreeVisitorImpl::updateParents(AstNode * const & curNode) {
	for (auto & curChild : curNode->children) {
		curChild->parent = curNode;
	}
}

/** Check if the actual parameters (arguments) here match type composition of formal parameters in function declaration. */
bool ParseTreeVisitorImpl::validateFunctionParameterTypes(AstNode * const & callNode) const {
	/** If there are zero arguments, type is void. If there are more than zero arguments, all types should be int. */
	if (callNode->symbolTableRecord->numArguments == callNode->children.size()) {
		return true;
	} else {
		// Print an error saying function parameter type mismatch.
		this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::NO_MATCHING_SIGNATURE, callNode->symbolTableRecord->token->getLine(), 
				("Arguments provided for function call do not match formal parameters in function definition : " + callNode->symbolTableRecord->text));
		return false;
	}
}


/** Update information about the input() and output() functions in the symbol table. They have the following signatures:
int input(void) { ... }
void output(int x) { ... }
*/
void ParseTreeVisitorImpl::populateLanguageSpecificFunctionInfo() {
	// Check if this input file used the input() function in the first place.
	for (auto const & curToken : this->compiler->getTokenStream()->getTokens()) {
		if (curToken->getText() == "input") {
			// Update the input() function.
			std::shared_ptr<SymbolRecord> inputFunctionSymbolRecord = std::make_shared<SymbolRecord>(curToken);
			inputFunctionSymbolRecord->text = "input";
			inputFunctionSymbolRecord->type = CMINUS_NATIVE_TYPES::INT;
			inputFunctionSymbolRecord->returnType = CMINUS_NATIVE_TYPES::INT;
			inputFunctionSymbolRecord->kind = SYMBOL_RECORD_KIND::FUNCTION;
			inputFunctionSymbolRecord->isDeclared = true;
			inputFunctionSymbolRecord->isDefined = true;
			inputFunctionSymbolRecord->numArguments = 0;
			this->compiler->getSymbolTableForCurrentContext()->insertSymbol(inputFunctionSymbolRecord);
		}
	}
	// Check if this input file used the output() function in the first place.
	for (auto const & curToken : this->compiler->getTokenStream()->getTokens()) {
		if (curToken->getText() == "output") {
			// Update the output() function.
			std::shared_ptr<SymbolRecord> outputFunctionSymbolRecord = std::make_shared<SymbolRecord>(curToken);
			outputFunctionSymbolRecord->text = "output";
			outputFunctionSymbolRecord->type = CMINUS_NATIVE_TYPES::VOID;
			outputFunctionSymbolRecord->returnType = CMINUS_NATIVE_TYPES::VOID;
			outputFunctionSymbolRecord->kind = SYMBOL_RECORD_KIND::FUNCTION;
			outputFunctionSymbolRecord->isDeclared = true;
			outputFunctionSymbolRecord->isDefined = true;
			outputFunctionSymbolRecord->numArguments = 1;
			this->compiler->getSymbolTableForCurrentContext()->insertSymbol(outputFunctionSymbolRecord);
		}
	}
}


/** Define a custom visitor for the declaration list visitor to make a list of declarations. */
antlrcpp::Any ParseTreeVisitorImpl::visitProgram(AntlrGrammarGenerated::TParser::ProgramContext *ctx) {
	/** Before starting AST, populate info about language-specific functions, input() and output(). */
	this->populateLanguageSpecificFunctionInfo();
	/** Create the root node of the AST, the ProgramContext rule node. */
	AstNode * programNode = new AstNode(ctx, this->compiler);
	/** Set the vector of Declaration nodes as the children of the Program node. */
	AstNode::AstNodePtrVectorType declNodeVector = this->visitChildren(ctx);
	programNode->children = declNodeVector;
	// Set the parents of this node's children.
	this->updateParents(programNode);
	return programNode;
}

/** Define a custom visitor for the declaration list visitor to make a list of declarations. */
antlrcpp::Any ParseTreeVisitorImpl::visitDeclarationList(AntlrGrammarGenerated::TParser::DeclarationListContext * ctx) {
	// Traverse the tree to populate a vector of declaration AstNode nodes. 
	return this->populateChildrenFromList<AntlrGrammarGenerated::TParser::DeclarationContext, AntlrGrammarGenerated::TParser::DeclarationListContext>(ctx);
}

/** Define a custom visitor for the VarDeclaration visitor. */
antlrcpp::Any ParseTreeVisitorImpl::visitVarDeclaration(AntlrGrammarGenerated::TParser::VarDeclarationContext * ctx) {
	AstNode * varDeclNode = new AstNode(ctx, this->compiler);
	// Save the information for the ID.
	std::shared_ptr<SymbolRecord> idSymbolRecord = std::make_shared<SymbolRecord>(ctx->ID()->getSymbol());
	// Declaration and definition of variables of primitive types is synonymous.
	idSymbolRecord->isDeclared = true;
	idSymbolRecord->isDefined = true;
	// Exchange pointers for association of this node with this symbol table entry.
	idSymbolRecord->astNode = std::make_shared<AstNode>(varDeclNode);
	varDeclNode->symbolTableRecord = idSymbolRecord;
	// Check to make sure this variable declaration is an INT.
	std::string variableType = this->visit(ctx->children.at(0));
	if (variableType == "int") {
		// All variable declarations should be integer types. 
		idSymbolRecord->type = CMINUS_NATIVE_TYPES::INT;
	} else {
		// VOID is illegal for variable declarations. 
		this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::INVALID_TYPE, idSymbolRecord->token->getLine(),
				("Illegal type used for variable declaration: " + idSymbolRecord->text));
	}
	// Check if we have have an integer or array variable declaration.
	if (ctx->children.size() < 4) {
		idSymbolRecord->kind = SYMBOL_RECORD_KIND::VARIABLE;
		idSymbolRecord->storageSize = sizeof(int);
	} else {
		idSymbolRecord->kind = SYMBOL_RECORD_KIND::ARRAY;
		// Storage size in memory is sizeof(int) * numElements.
		idSymbolRecord->storageSize = sizeof(int) * std::stoi(ctx->NUM()->getSymbol()->getText());
		// Add the new NUM symbol to the symbol table for the current scope.
		this->compiler->getSymbolTableForCurrentContext()->emplaceSymbol(ctx->NUM()->getSymbol());
	}
	// Add the new ID symbol to the symbol table for the current scope.
	this->compiler->getSymbolTableForCurrentContext()->insertSymbol(idSymbolRecord);
	// Update the storage location of this variable, based on the storage size.
	this->compiler->getSemanticAnalyzer()->storeRecordInMemory(idSymbolRecord);
	return varDeclNode;
}

/** Define a custom visitor for the FunDeclaration visitor. */
antlrcpp::Any ParseTreeVisitorImpl::visitFunDeclaration(AntlrGrammarGenerated::TParser::FunDeclarationContext * ctx) {
	AstNode * funDeclNode = new AstNode(ctx, this->compiler);
	// Push a new function scope into the SymbolTableManager, for function params and subsequent declarations.
	this->compiler->getSymbolTableManager()->newScope();
	// Save the function parameters (in nested scope).
	AstNode * paramsNode = this->visit(ctx->children.at(3));
	funDeclNode->children.push_back(paramsNode);
	// Create a new symbol table record for the function ID information.
	std::shared_ptr<SymbolRecord> idSymbolRecord = std::make_shared<SymbolRecord>(ctx->ID()->getSymbol());
	// Visit the type specifier node to get the function's return type.
	std::string functionReturnType = this->visit(ctx->children.at(0));
	if (functionReturnType == "int") {
		idSymbolRecord->type = CMINUS_NATIVE_TYPES::INT;
		idSymbolRecord->returnType = CMINUS_NATIVE_TYPES::INT;
	} else {
		idSymbolRecord->type = CMINUS_NATIVE_TYPES::VOID;
		idSymbolRecord->returnType = CMINUS_NATIVE_TYPES::VOID;
	}
	idSymbolRecord->kind = SYMBOL_RECORD_KIND::FUNCTION;
	// When a C-Minus function is declared, it is also defined.
	idSymbolRecord->isDeclared = true;
	idSymbolRecord->isDefined = true;
	// Populate the number of arguments for this function.
	idSymbolRecord->numArguments = paramsNode->children.size();
	// Exchange pointers for association of this node with this symbol table entry.
	idSymbolRecord->astNode = std::make_shared<AstNode>(funDeclNode);
	funDeclNode->symbolTableRecord = idSymbolRecord;
	// Add the new ID symbol to the symbol table for the enclosing scope (function declaration is one scope above its params).
	this->compiler->getSymbolTableManager()->getCurrentScope()->enclosingScope->scopedSymbolTable->insertSymbol(idSymbolRecord);
	// Save the function body compound statement (which may contain recursive calls to this function).
	AstNode * compoundStatementNode = this->visit(ctx->children.at(5));
	funDeclNode->children.push_back(compoundStatementNode);
	// Set the parents of this node's children.
	this->updateParents(funDeclNode);
	return funDeclNode;
}

/** Define a custom visitor for the type specifier. */
antlrcpp::Any ParseTreeVisitorImpl::visitTypeSpecifier(AntlrGrammarGenerated::TParser::TypeSpecifierContext * ctx) {
	return dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children.at(0))->getText();
}

/** Define a custom visitor for the params node. */
antlrcpp::Any ParseTreeVisitorImpl::visitParams(AntlrGrammarGenerated::TParser::ParamsContext * ctx) {
	AstNode * paramsNode = new AstNode(ctx, this->compiler);
	// Check if we have any parameters, a terminal node will indicate VOID or no params.
	if (!antlrcpp::is<antlr4::tree::TerminalNode *>(ctx->children.at(0))) {
		AstNode::AstNodePtrVectorType childNodes = this->visit(ctx->children.at(0));
		paramsNode->children = childNodes;
		// Set the parents of this node's children.
		this->updateParents(paramsNode);
	}
	return paramsNode;
}

/** Define a custom visitor for the params node. */
antlrcpp::Any ParseTreeVisitorImpl::visitParamList(AntlrGrammarGenerated::TParser::ParamListContext * ctx) {
	// Traverse the tree to populate a vector of param AstNode nodes. 
	return this->populateChildrenFromList<AntlrGrammarGenerated::TParser::ParamContext, AntlrGrammarGenerated::TParser::ParamListContext>(ctx);
}

/** Define a custom visitor for the params node. */
antlrcpp::Any ParseTreeVisitorImpl::visitParam(AntlrGrammarGenerated::TParser::ParamContext * ctx) {
	AstNode * paramNode = new AstNode(ctx, this->compiler);
	// Save the information for the ID.
	std::shared_ptr<SymbolRecord> idSymbolRecord = std::make_shared<SymbolRecord>(ctx->ID()->getSymbol());
	// Declaration and definition of parameters of primitive types is synonymous.
	idSymbolRecord->isDeclared = true;
	idSymbolRecord->isDefined = true;
	// Exchange pointers for association of this node with this symbol table entry.
	idSymbolRecord->astNode = std::make_shared<AstNode>(paramNode);
	paramNode->symbolTableRecord = idSymbolRecord;
	// Visit the typeSpecifier node to verify this is an INT type. All function parameters should be INT!
	std::string functionParamType = this->visit(ctx->children.at(0));
	if (functionParamType == "int") {
		// All parameters should be integer types. 
		idSymbolRecord->type = CMINUS_NATIVE_TYPES::INT;
	} else {
		// VOID type is illegal for function parameters. 
		this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::INVALID_TYPE, ctx->ID()->getSymbol()->getLine(),
				("Illegal type used for function parameter: void " + ctx->ID()->getSymbol()->getText()));
	}
	// Check if we have have an integer or array parameter declaration.
	if (ctx->children.size() < 3) {
		idSymbolRecord->kind = SYMBOL_RECORD_KIND::VARIABLE;
		idSymbolRecord->storageSize = sizeof(int);
	} else {
		idSymbolRecord->kind = SYMBOL_RECORD_KIND::ARRAY;
	}
	// Add the new ID symbol to the symbol table for the current scope.
	this->compiler->getSymbolTableForCurrentContext()->insertSymbol(idSymbolRecord);
	// TODO: Associate get the corresponding variable decl for this param, so we can update its storage location.
	return paramNode;
}

/** Define a custom visitor for the compound statement. */
antlrcpp::Any ParseTreeVisitorImpl::visitCompoundStmt(AntlrGrammarGenerated::TParser::CompoundStmtContext * ctx) {
	AstNode * compoundStatementNode = new AstNode(ctx, this->compiler);
	// Build a local declarations node, whose children are variable declarations.
	AstNode * localDeclsNode = new AstNode(ctx->children.at(1), this->compiler); 
	AstNode::AstNodePtrVectorType varDeclsVector = this->visit(ctx->children.at(1));
	localDeclsNode->children = varDeclsVector;
	// Set the parents of this localDeclsNode's children.
	this->updateParents(localDeclsNode);
	compoundStatementNode->children.push_back(localDeclsNode);
	// Build a statement list node, whose children are function statements.
	AstNode * statementListNode = new AstNode(ctx->children.at(2), this->compiler);
	AstNode::AstNodePtrVectorType statementsVector = this->visit(ctx->children.at(2));
	statementListNode->children = statementsVector;
	// Set the parents of this statementListNode's children.
	this->updateParents(statementListNode);
	compoundStatementNode->children.push_back(statementListNode);
	// Set the parents of this node's children.
	this->updateParents(compoundStatementNode);
	// Pop the function body scope from the SymbolTableManager.
	this->compiler->getSymbolTableManager()->popScope();
	return compoundStatementNode;
}

/** Define a custom visitor for the local function variable declarations (zero or many). */
antlrcpp::Any ParseTreeVisitorImpl::visitLocalDeclaration(AntlrGrammarGenerated::TParser::LocalDeclarationContext * ctx) {
	AstNode::AstNodePtrVectorType varDeclNodes;
	// Check if there are any local declarations for this function.
	if (!ctx->children.empty()) {
		// Traverse the tree to populate a vector of variable declaration AstNode nodes. 
		varDeclNodes = this->populateChildrenFromList<AntlrGrammarGenerated::TParser::VarDeclarationContext, AntlrGrammarGenerated::TParser::LocalDeclarationContext>(ctx);
	}
	return varDeclNodes;
}

/** Define a custom visitor for the function statements (zero or many). */
antlrcpp::Any ParseTreeVisitorImpl::visitStatementList(AntlrGrammarGenerated::TParser::StatementListContext * ctx) {
	AstNode::AstNodePtrVectorType statementNodes;
	// Check if there are any statements for this function body.
	if (!ctx->children.empty()) {
		// Traverse the tree to populate a vector of statement AstNode nodes. 
		statementNodes = this->populateChildrenFromList<AntlrGrammarGenerated::TParser::StatementContext, AntlrGrammarGenerated::TParser::StatementListContext>(ctx);
	}
	return statementNodes;
}

antlrcpp::Any ParseTreeVisitorImpl::visitExpressionStmt(AntlrGrammarGenerated::TParser::ExpressionStmtContext * ctx) {
	// Check if this is an empty expression.
	if (antlrcpp::is<antlr4::tree::TerminalNode *>(ctx->children.at(0))) {
		// Return an AstNode representing an empty expression statement. This gets pruned out of the tree in AST walk.
		return new AstNode(ctx, this->compiler);
	}
	// Otherwise, visit the expression and save its information.
	else {
		return this->visit(ctx->children.at(0));
	}
}

antlrcpp::Any ParseTreeVisitorImpl::visitSelectionStmt(AntlrGrammarGenerated::TParser::SelectionStmtContext * ctx) {
	AstNode * selectionStatementNode = new AstNode(ctx, this->compiler);
	// Check number of children to see if an ELSE is used.
	if (ctx->children.size() == 5) {
		// Save expression and statement as children.
		selectionStatementNode->children.push_back(this->visit(ctx->children.at(2)));
		selectionStatementNode->children.push_back(this->visit(ctx->children.at(4)));
	}
	// Process an ELSE statement also.
	else {
		// Save expression and 2 statements as children.
		selectionStatementNode->children.push_back(this->visit(ctx->children.at(2)));
		selectionStatementNode->children.push_back(this->visit(ctx->children.at(4)));
		selectionStatementNode->children.push_back(this->visit(ctx->children.at(6)));
	}
	// Set the parents of this node's children.
	this->updateParents(selectionStatementNode);
	return selectionStatementNode;
}

antlrcpp::Any ParseTreeVisitorImpl::visitIterationStmt(AntlrGrammarGenerated::TParser::IterationStmtContext * ctx) {
	AstNode * iterationStatementNode = new AstNode(ctx, this->compiler);
	// Save expression and statement.
	iterationStatementNode->children.push_back(this->visit(ctx->children.at(2)));
	iterationStatementNode->children.push_back(this->visit(ctx->children.at(4)));
	// Set the parents of this node's children.
	this->updateParents(iterationStatementNode);
	return iterationStatementNode;
}

antlrcpp::Any ParseTreeVisitorImpl::visitReturnStmt(AntlrGrammarGenerated::TParser::ReturnStmtContext * ctx) {
	AstNode * returnNode = new AstNode(ctx, this->compiler);
	// Check if there is any return value.
	if (ctx->children.size() == 3) {
		// Visit the expression node and save return expression as child.
		AstNode * expressionNode = this->visit(ctx->children.at(1));
		returnNode->children.push_back(expressionNode);
		// Set the parents of this node's children.
		this->updateParents(returnNode);
	}
	// Save the return keyword for reference later.
	else {
		returnNode->token = ctx->RETURN_KEYWORD()->getSymbol();
	}
	return returnNode;
}

/** Define a custom visitor for the Expression node. */
antlrcpp::Any ParseTreeVisitorImpl::visitExpression(AntlrGrammarGenerated::TParser::ExpressionContext * ctx) {
	AstNode * expressionNode = new AstNode(ctx, this->compiler);
	// Check if this is an assignment expression (3 children).
	if (ctx->children.size() == 3) {
		// Visit the var node.
		expressionNode->children.push_back(this->visit(ctx->children.at(0)));
		// Visit the expression node.
		expressionNode->children.push_back(this->visit(ctx->children.at(2)));
		// Ensure the variable is declared and defined before we assign to it!
		if (expressionNode->children.at(0)->symbolTableRecord->isDeclared && expressionNode->children.at(0)->symbolTableRecord->isDefined) {
			// Designate the variable as "assigned" in the symbol table.
			expressionNode->children.at(0)->symbolTableRecord->isAssigned = true;
		} else {
			// Cannot assign a value to undefined variable.
			this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::UNDECL_IDENTIFIER, 
					expressionNode->children.at(0)->symbolTableRecord->token->getLine(), 
					("Undeclared identifier being assigned to: " + expressionNode->children.at(0)->symbolTableRecord->text));
		}
	}
	else {
		// Visit the simpleExpression node, saving it instead of this node.
		expressionNode = this->visitChildren(ctx);
	}
	// Set the parents of this node's children.
	this->updateParents(expressionNode);
	return expressionNode;
}

/** Define a custom visitor for the variable node. */
antlrcpp::Any ParseTreeVisitorImpl::visitVar(AntlrGrammarGenerated::TParser::VarContext * ctx) {
	AstNode * varNode = new AstNode(ctx, this->compiler);
	// Lookup this variable in the current scope's symbol table, then in enclosing scopes.
	auto matchingSymbol = this->compiler->getSymbolTableManager()->getCurrentScope()->findSymbol(ctx->ID()->getText());
	if (matchingSymbol) {
		// Link this node to its corresponding ID entry in the symbol table.
		varNode->symbolTableRecord = matchingSymbol;
		// Check if this is an array, not an integer.
		if (ctx->children.size() > 1) {
			// Visit the expression child to get the index into the array.
			AstNode * expressionNode = this->visit(ctx->children.at(2));
			// Save the expression as a child to be evaluated later.
			varNode->children.push_back(expressionNode);
			// Set the parents of this node's children.
			this->updateParents(varNode);
			// Since this is an array-indexed variable, check that it has a matching array variable declaration.
			if (matchingSymbol->kind != SYMBOL_RECORD_KIND::ARRAY) {
				// Print an error.
				this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::INVALID_SYNTAX,
						ctx->ID()->getSymbol()->getLine(), ("The use of subscripts with variable "
								+ ctx->ID()->getSymbol()->getText() + " requires the variable be an array type."));
			}
		}
	} else {
		// Cannot use an undeclared variable.
		this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::UNDECL_IDENTIFIER,
				ctx->ID()->getSymbol()->getLine(), ("Undeclared identifier: " + ctx->ID()->getSymbol()->getText()));
	}
	// Variables are the only C-Minus entity that can be L-Values.
	return varNode;
}

/** Define a custom visitor for the simple expression node. */
antlrcpp::Any ParseTreeVisitorImpl::visitSimpleExpression(AntlrGrammarGenerated::TParser::SimpleExpressionContext * ctx) {
	// Process the children of this simple expression node.
	return this->visitAndCollapseExpressionNodes<AntlrGrammarGenerated::TParser::SimpleExpressionContext>(ctx);
}

/** Define a custom visitor for the additive expression node. */
antlrcpp::Any ParseTreeVisitorImpl::visitAdditiveExpression(AntlrGrammarGenerated::TParser::AdditiveExpressionContext * ctx) {
	// Process the children of this additive expression node.
	return this->visitAndCollapseExpressionNodes<AntlrGrammarGenerated::TParser::AdditiveExpressionContext>(ctx);
}

/** Define a custom visitor for the term node. */
antlrcpp::Any ParseTreeVisitorImpl::visitTerm(AntlrGrammarGenerated::TParser::TermContext * ctx) {
	// Process the children of this term node.
	return this->visitAndCollapseExpressionNodes<AntlrGrammarGenerated::TParser::TermContext>(ctx);
}

/** Define a custom visitor for the factor node. */
antlrcpp::Any ParseTreeVisitorImpl::visitFactor(AntlrGrammarGenerated::TParser::FactorContext * ctx) {
	AstNode * factorNode = new AstNode(ctx, this->compiler);
	// Check for a nested expression (nested in parentheses).
	if (ctx->children.size() > 1) {
		// Visit the nested expression node, saving it in place of this node.
		factorNode = this->visit(ctx->children.at(1));
	} else {
		// Check if the one child is simply a number.
		if (antlrcpp::is<antlr4::tree::TerminalNode *>(ctx->children.at(0))) {
			// Add the new NUM symbol to the symbol table for the current scope.
			this->compiler->getSymbolTableForCurrentContext()->emplaceSymbol(ctx->NUM()->getSymbol(), factorNode);
		} else {
			// Visit the call/var node, saving it in place of this node.
			factorNode = this->visitChildren(ctx);
		}
	}
	// Set the parents of this node's children.
	this->updateParents(factorNode);
	return factorNode;
}

/** Define a custom visitor for the call node. */
antlrcpp::Any ParseTreeVisitorImpl::visitCall(AntlrGrammarGenerated::TParser::CallContext * ctx) {
	AstNode * callNode = new AstNode(ctx, this->compiler);
	// Get the function name (ID) and ensure it is defined in this scope.
	if (this->compiler->getSymbolTableManager()->getCurrentScope()->findSymbol(ctx->ID()->getText())) {
		// Exchange pointers for association of this node with this symbol table entry.
		callNode->symbolTableRecord = this->compiler->getSymbolTableManager()->getCurrentScope()->findSymbol(ctx->ID()->getText());
	} else {
		// Otherwise, this function is not yet declared.
		this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::UNDECL_IDENTIFIER,
				ctx->ID()->getSymbol()->getLine(), ("Undeclared identifier being used for function call: " + ctx->ID()->getText()));
	}
	// Get the arguments for the function call and save them as children.
	AstNode::AstNodePtrVectorType argumentsVector = this->visit(ctx->children.at(2));
	callNode->children = argumentsVector;
	// Check if the actual parameters (arguments) here match type composition of formal parameters in function declaration.
	// TODO: This will fail if argument to function call is another function call....
	this->validateFunctionParameterTypes(callNode);
	// Set the parents of this node's children.
	this->updateParents(callNode);
	return callNode;
}

/** Define a custom visitor for the arguments node. */
antlrcpp::Any ParseTreeVisitorImpl::visitArgs(AntlrGrammarGenerated::TParser::ArgsContext * ctx) {
	// Check if there are any arguments for the function call.
	AstNode::AstNodePtrVectorType argumentsVector;
	if (!ctx->children.empty()) {
		return this->visit(ctx->children.at(0));
	}
	return argumentsVector;
}

/** Define a custom visitor for the argument list to create a vector of arguments. */
antlrcpp::Any ParseTreeVisitorImpl::visitArgList(AntlrGrammarGenerated::TParser::ArgListContext * ctx) {
	// Traverse the tree to populate a vector of argument AstNode nodes. 
	return this->populateChildrenFromList<AntlrGrammarGenerated::TParser::ExpressionContext, AntlrGrammarGenerated::TParser::ArgListContext>(ctx);
}



