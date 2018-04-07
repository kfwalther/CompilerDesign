/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "ParseTreeVisitorImpl.h"
#include "ErrorHandler.h"

/** Define the default constructor for the Parse Tree Listener. */
ParseTreeVisitorImpl::ParseTreeVisitorImpl(AntlrGrammarGenerated::TParser * const parser) : parser(parser) {
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
		// TODO: Improve this error message a bit. Describe line in input where error exists.
		std::cerr << ErrorHandler::ErrorCodes::NO_MATCHING_SIGNATURE << std::endl;
		std::cerr << "Arguments provided for function call do not match formal parameters in function definition: "
				<< callNode->symbolTableRecord->text << std::endl << std::endl;
		return false;
	}
}


/** Update information about the input() and output() functions in the symbol table. They have the following signatures:
int input(void) { ... }
void output(int x) { ... }
*/
void ParseTreeVisitorImpl::populateLanguageSpecificFunctionInfo() {
	// Check if this input file used the input() function in the first place.
	if (this->parser->getSymbolTable()->symbolTable.count("input")) {
		// Update the input() function.
		this->parser->getSymbolTable()->symbolTable["input"]->type = CMINUS_NATIVE_TYPES::INT;
		this->parser->getSymbolTable()->symbolTable["input"]->returnType = CMINUS_NATIVE_TYPES::INT;
		this->parser->getSymbolTable()->symbolTable["input"]->kind = SYMBOL_RECORD_KIND::FUNCTION;
		this->parser->getSymbolTable()->symbolTable["input"]->isDeclared = true;
		this->parser->getSymbolTable()->symbolTable["input"]->isDefined = true;
		this->parser->getSymbolTable()->symbolTable["input"]->numArguments = 0;
	}
	// Check if this input file used the output() function in the first place.
	if (this->parser->getSymbolTable()->symbolTable.count("output")) {
		// Update the output() function.
		this->parser->getSymbolTable()->symbolTable["output"]->type = CMINUS_NATIVE_TYPES::VOID;
		this->parser->getSymbolTable()->symbolTable["output"]->returnType = CMINUS_NATIVE_TYPES::VOID;
		this->parser->getSymbolTable()->symbolTable["output"]->kind = SYMBOL_RECORD_KIND::FUNCTION;
		this->parser->getSymbolTable()->symbolTable["output"]->isDeclared = true;
		this->parser->getSymbolTable()->symbolTable["output"]->isDefined = true;
		this->parser->getSymbolTable()->symbolTable["output"]->numArguments = 1;
	}
}


/** Define a custom visitor for the declaration list visitor to make a list of declarations. */
antlrcpp::Any ParseTreeVisitorImpl::visitProgram(AntlrGrammarGenerated::TParser::ProgramContext *ctx) {
	/** Before starting AST, populate info about language-specific functions, input() and output(). */
	this->populateLanguageSpecificFunctionInfo();
	/** Create the root node of the AST, the ProgramContext rule node. */
	AstNode * programNode = new AstNode(ctx);
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
	AstNode * varDeclNode = new AstNode(ctx);
	auto symbolTableIterator = this->parser->getSymbolTable()->symbolTable.begin();
	// Loop through all children and save AST node. 
	for (auto const & child : ctx->children) {
		// Find the ID/NUM nodes that declare the variable. 
		if (antlrcpp::is<antlr4::tree::TerminalNode *>(child)) {
			auto curNode = dynamic_cast<antlr4::tree::TerminalNode *>(child);
			auto nodeType = (dynamic_cast<antlr4::tree::TerminalNode *>(child))->getSymbol()->getType();
			// Update symbol table info if we have the ID.
			if (nodeType == this->parser->ID) {
				// Find the associated entry in the symbol table and update its information.
				symbolTableIterator = this->parser->getSymbolTable()->symbolTable.find(curNode->getSymbol()->getText());
				if (symbolTableIterator != this->parser->getSymbolTable()->symbolTable.end()) {
					// Assume we have just a single integer declaration here, can update for array afterward.
					symbolTableIterator->second->storageSize = sizeof(int);
					symbolTableIterator->second->kind = SYMBOL_RECORD_KIND::VARIABLE;
					// Declaration and definition of variables of primitive types is synonymous.
					symbolTableIterator->second->isDeclared = true;
					symbolTableIterator->second->isDefined = true;
					// Exchange pointers for association of this node with this symbol table entry.
					symbolTableIterator->second->astNode = std::make_shared<AstNode>(varDeclNode);
					varDeclNode->symbolTableRecord = symbolTableIterator->second;
					// Check to make sure this variable declaration is an INT.
					std::string variableType = this->visit(ctx->children.at(0));
					if (variableType == "int") {
						// All variable declarations will be integer types. 
						symbolTableIterator->second->type = CMINUS_NATIVE_TYPES::INT;
					} else {
						// VOID is illegal for variable declarations. 
						// TODO: Improve this error message a bit. Describe line in input where error exists.
						std::cerr << ErrorHandler::ErrorCodes::INVALID_TYPE << std::endl;
						std::cerr << "Illegal type used for variable declaration: " << symbolTableIterator->second->text << std::endl << std::endl;
					}
				}
				else {
					std::cerr << ErrorHandler::ErrorCodes::COMPILER_ERROR << std::endl;
					std::cerr << "visitVarDeclaration: AST visit encountered Token not in symbol table yet. "
							<< "This typically indicates a problem with the ParseTreeListener." << std::endl << std::endl;
				}
			}
			// If we find a NUM token, we know we have an array variable declaration.
			else if (nodeType == this->parser->NUM) {
				symbolTableIterator->second->kind = SYMBOL_RECORD_KIND::ARRAY;
				// Storage size in memory is sizeof(int) * numElements.
				symbolTableIterator->second->storageSize = sizeof(int) * std::stoi(curNode->getSymbol()->getText());
			}
		}
	}
	return varDeclNode;
}

/** Define a custom visitor for the FunDeclaration visitor. */
antlrcpp::Any ParseTreeVisitorImpl::visitFunDeclaration(AntlrGrammarGenerated::TParser::FunDeclarationContext * ctx) {
	AstNode * funDeclNode = new AstNode(ctx);	
	// Save the function parameters.
	AstNode * paramsNode = this->visit(ctx->children.at(3));
	funDeclNode->children.push_back(paramsNode);
	// Save the function body compound statement.
	AstNode * compoundStatementNode = this->visit(ctx->children.at(5));
	funDeclNode->children.push_back(compoundStatementNode);
	// Find the associated ID entry in the symbol table and update its information.
	auto symbolTableIterator = this->parser->getSymbolTable()->symbolTable.find(ctx->ID()->getSymbol()->getText());
	if (symbolTableIterator != this->parser->getSymbolTable()->symbolTable.end()) {
		// Visit the type specifier node to get the function's return type.
		std::string functionReturnType = this->visit(ctx->children.at(0));
		if (functionReturnType == "int") {
			symbolTableIterator->second->type = CMINUS_NATIVE_TYPES::INT;
			symbolTableIterator->second->returnType = CMINUS_NATIVE_TYPES::INT;
		} else {
			symbolTableIterator->second->type = CMINUS_NATIVE_TYPES::VOID;
			symbolTableIterator->second->returnType = CMINUS_NATIVE_TYPES::VOID;
		}
		symbolTableIterator->second->kind = SYMBOL_RECORD_KIND::FUNCTION;
		// When a C-Minus function is declared, it is also defined.
		symbolTableIterator->second->isDeclared = true;
		symbolTableIterator->second->isDefined = true;
		// Populate the number of arguments for this function.
		// TODO: Figure out if we have to track same function name with different num of args separately...
		symbolTableIterator->second->numArguments = paramsNode->children.size();
		// Exchange pointers for association of this node with this symbol table entry.
		symbolTableIterator->second->astNode = std::make_shared<AstNode>(funDeclNode);
		funDeclNode->symbolTableRecord = symbolTableIterator->second;
	}
	else {
		std::cerr << ErrorHandler::ErrorCodes::COMPILER_ERROR << std::endl;
		std::cerr << "visitFunDeclaration: AST visit encountered Token not in symbol table yet. "
				<< "This typically indicates a problem with the ParseTreeListener." << std::endl << std::endl;
	}
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
	AstNode * paramsNode = new AstNode(ctx);
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
// TODO: Maybe consider reorganizing this function. Removing the loop....
antlrcpp::Any ParseTreeVisitorImpl::visitParam(AntlrGrammarGenerated::TParser::ParamContext * ctx) {
	AstNode * paramNode = new AstNode(ctx);
	auto symbolTableIterator = this->parser->getSymbolTable()->symbolTable.begin();
	// Loop through all children and populate this AST node. 
	for (auto const & child : ctx->children) {
		// Find the ID or BRACKET nodes that define the parameter. 
		if (antlrcpp::is<antlr4::tree::TerminalNode *>(child)) {
			auto nodeType = (dynamic_cast<antlr4::tree::TerminalNode *>(child))->getSymbol()->getType();
			// Save some info from the parameter ID node.
			if (nodeType == this->parser->ID) {
				// Find the associated entry in the symbol table and update its information.
				symbolTableIterator = this->parser->getSymbolTable()->symbolTable.find(ctx->ID()->getSymbol()->getText());
				if (symbolTableIterator != this->parser->getSymbolTable()->symbolTable.end()) {
					// All parameter declarations will be integer types. 
					symbolTableIterator->second->type = CMINUS_NATIVE_TYPES::INT;
					// Assume we have a simple integer parameter, can update later if it is actually an array parameter.
					symbolTableIterator->second->kind = SYMBOL_RECORD_KIND::VARIABLE;
					symbolTableIterator->second->storageSize = 4;
					// Declaration and definition of parameters of primitive types is synonymous.
					symbolTableIterator->second->isDeclared = true;
					symbolTableIterator->second->isDefined = true;
					// Exchange pointers for association of this node with this symbol table entry.
					symbolTableIterator->second->astNode = std::make_shared<AstNode>(paramNode);
					paramNode->symbolTableRecord = symbolTableIterator->second;
				} else {
					std::cerr << ErrorHandler::ErrorCodes::COMPILER_ERROR << std::endl;
					std::cerr << "visitParam: AST visit encountered Token not in symbol table yet. "
							<< "This typically indicates a problem with the ParseTreeListener." << std::endl << std::endl;
				}
			}
			// If we see the brackets, we know this is an array parameter.
			else if (nodeType == this->parser->LEFT_BRACKET) {
				symbolTableIterator->second->kind = SYMBOL_RECORD_KIND::ARRAY;
				/* TODO: Need more context to update storage size of array parameter, need to lookup function
					declaration and get original array param name passed. */
				// TODO: Functionize this.
				//symbolTableIterator->second->storageSize = 4;
			}
		}
		// Visit the typeSpecifier node to verify this is an INT type. All function parameters should be INT!
		else {
			std::string functionReturnType = this->visit(child);
			if (functionReturnType == "void") {
				// TODO: Put better error reporting info here...
				std::cerr << ErrorHandler::ErrorCodes::INVALID_TYPE << std::endl;
				std::cerr << "Illegal type used for function parameter: void " 
						<< ctx->ID()->getSymbol()->getText() << std::endl << std::endl;
			}
		}
	}
	return paramNode;
}

/** Define a custom visitor for the compound statement. */
antlrcpp::Any ParseTreeVisitorImpl::visitCompoundStmt(AntlrGrammarGenerated::TParser::CompoundStmtContext * ctx) {
	AstNode * compoundStatementNode = new AstNode(ctx);
	// Build a local declarations node, whose children are variable declarations.
	AstNode * localDeclsNode = new AstNode(ctx->children.at(1)); 
	AstNode::AstNodePtrVectorType varDeclsVector = this->visit(ctx->children.at(1));
	localDeclsNode->children = varDeclsVector;
	// Set the parents of this localDeclsNode's children.
	this->updateParents(localDeclsNode);
	compoundStatementNode->children.push_back(localDeclsNode);
	// Build a statement list node, whose children are function statements.
	AstNode * statementListNode = new AstNode(ctx->children.at(2));
	AstNode::AstNodePtrVectorType statementsVector = this->visit(ctx->children.at(2));
	statementListNode->children = statementsVector;
	// Set the parents of this statementListNode's children.
	this->updateParents(statementListNode);
	compoundStatementNode->children.push_back(statementListNode);
	// Set the parents of this node's children.
	this->updateParents(compoundStatementNode);
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
		// Return an AstNode representing an empty expression statement.
		// TODO: Prune this out in the AST walk later.
		return new AstNode(ctx);
	}
	// Otherwise, visit the expression and save its information.
	else {
		return this->visit(ctx->children.at(0));
	}
}

antlrcpp::Any ParseTreeVisitorImpl::visitSelectionStmt(AntlrGrammarGenerated::TParser::SelectionStmtContext * ctx) {
	AstNode * selectionStatementNode = new AstNode(ctx);
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
	AstNode * iterationStatementNode = new AstNode(ctx);
	// Save expression and statement.
	iterationStatementNode->children.push_back(this->visit(ctx->children.at(2)));
	iterationStatementNode->children.push_back(this->visit(ctx->children.at(4)));
	// Set the parents of this node's children.
	this->updateParents(iterationStatementNode);
	return iterationStatementNode;
}

antlrcpp::Any ParseTreeVisitorImpl::visitReturnStmt(AntlrGrammarGenerated::TParser::ReturnStmtContext * ctx) {
	AstNode * returnNode = new AstNode(ctx);
	// Check if there is any return value.
	if (ctx->children.size() == 3) {
		// Visit the expression node and save return expression as child.
		AstNode * expressionNode = this->visit(ctx->children.at(1));
		if (expressionNode->symbolTableRecord->canBeUsed()) {
			expressionNode->symbolTableRecord->isUsed = true;
			// TODO: Check if the return type matches the function signature. This is easier to do via AST walk.
		} else {
			// Illegal to use before declared or assigned!
			// TODO: Improve this error message a bit. Describe line in input where error exists.
			// TODO: Are we allowed to return an unassigned value? Maybe we can...
			// TODO: Move this error to AST walk when we have more info...
			//std::cerr << ErrorHandler::ErrorCodes::UNDECL_IDENTIFIER << std::endl;
			//std::cerr << "Undeclared or unassigned expression returned from function!" << std::endl << std::endl;
		}
		returnNode->children.push_back(expressionNode);
		// Set the parents of this node's children.
		this->updateParents(returnNode);
	}
	return returnNode;
}

/** Define a custom visitor for the Expression node. */
antlrcpp::Any ParseTreeVisitorImpl::visitExpression(AntlrGrammarGenerated::TParser::ExpressionContext * ctx) {
	AstNode * expressionNode = new AstNode(ctx);
	// Check if this is an assignment expression (3 children).
	if (ctx->children.size() == 3) {
		// Visit the var node.
		expressionNode->children.push_back(this->visit(ctx->children.at(0)));
		// Visit the expression node.
		expressionNode->children.push_back(this->visit(ctx->children.at(2)));
		// TODO: Perform another check here to ensure r-value type matches l-value type. Do in AST walk.
		// Ensure the variable is declared and defined before we assign to it!
		if (expressionNode->children.at(0)->symbolTableRecord->isDeclared && expressionNode->children.at(0)->symbolTableRecord->isDefined) {
			// Designate the variable as "assigned" in the symbol table.
			expressionNode->children.at(0)->symbolTableRecord->isAssigned = true;
		} else {
			// Cannot assign a value to undefined variable.
			// TODO: Use token info to print the line in the file where this error occurred.
			std::cerr << ErrorHandler::ErrorCodes::UNDECL_IDENTIFIER << std::endl;
			std::cerr << "Undeclared identifier being assigned to : " << expressionNode->children.at(0)->symbolTableRecord->text << std::endl << std::endl;
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
	AstNode * varNode = new AstNode(ctx);
	// Link this node to its corresponding ID entry in the symbol table.
	// TODO: Make this a conditional lookup, and error if this variable definition is out of scope...
	varNode->symbolTableRecord = (this->parser->getSymbolTable()->symbolTable.find(ctx->ID()->getSymbol()->getText()))->second;
	// Check if this is an array, not an integer.
	if (ctx->children.size() > 1) {
		// Visit the expression child to get the index into the array.
		AstNode * expressionNode = this->visit(ctx->children.at(2));
		// Save the expression as a child to be evaluated later.
		varNode->children.push_back(expressionNode);
		// Set the parents of this node's children.
		this->updateParents(varNode);
		// TODO: This type is actually INT b/c it's an unknown index into array. How do we handle this?
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
	AstNode * factorNode = new AstNode(ctx);
	// Check for a nested expression (nested in parentheses).
	if (ctx->children.size() > 1) {
		// Visit the nested expression node, saving it in place of this node.
		factorNode = this->visit(ctx->children.at(1));
	} else {
		// Check if the one child is simply a number.
		if (antlrcpp::is<antlr4::tree::TerminalNode *>(ctx->children.at(0))) {
			// Find this number in the symbol table.
			auto symbolTableIterator = this->parser->getSymbolTable()->symbolTable.find(ctx->NUM()->getText());
			if (symbolTableIterator != this->parser->getSymbolTable()->symbolTable.end()) {
				// Store some info about this integer number. 
				symbolTableIterator->second->type = CMINUS_NATIVE_TYPES::INT;
				symbolTableIterator->second->kind = SYMBOL_RECORD_KIND::NUMBER;
				// Exchange pointers for association of this node with this symbol table entry.
				symbolTableIterator->second->astNode = std::make_shared<AstNode>(factorNode);
				factorNode->symbolTableRecord = symbolTableIterator->second;
			}
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
	AstNode * callNode = new AstNode(ctx);
	// Get the function name (ID) and store it in the symbol table if it doesn't already exist.
	auto symbolTableIterator = this->parser->getSymbolTable()->symbolTable.find(ctx->ID()->getText());
	if (symbolTableIterator != this->parser->getSymbolTable()->symbolTable.end()) {
		// Exchange pointers for association of this node with this symbol table entry.
		callNode->symbolTableRecord = symbolTableIterator->second;
	}
	// Get the arguments for the function call and save them as children.
	AstNode::AstNodePtrVectorType argumentsVector = this->visit(ctx->children.at(2));
	callNode->children = argumentsVector;
	// TODO: Check here to ensure none of the arguments are another function call. Do this in AST walk.
	// Check if the actual parameters (arguments) here match type composition of formal parameters in function declaration.
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



