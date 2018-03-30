/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "ParseTreeVisitorImpl.h"

/** Define the default constructor for the Parse Tree Listener. */
ParseTreeVisitorImpl::ParseTreeVisitorImpl(AntlrGrammarGenerated::TParser * const parser) : parser(parser) {
	return;
}

/** Define a helper function to support parse tree traversal of list nodes. */
template<class EntityType, class EntityListType>
AstNode::AstNodePtrVectorType ParseTreeVisitorImpl::populateChildrenFromList(EntityListType * ctx) {
	// Declare a vector of AstNodes to return.
	AstNode::AstNodePtrVectorType entityNodeVector;
	// Loop through all children (grammar is: Entity | EntityList Entity).
	for (auto const & curNode : ctx->children) {
		// If the current node is just a entity, add it to the entityNodeVector.
		if (antlrcpp::is<EntityType *>(curNode)) {
			// Visit the child first to create a node for specific type of entity.
			entityNodeVector.push_back(this->visit(curNode));
		}
		// Append the entity nodes from the nested EntityList to entityNodeVector.
		if (antlrcpp::is<EntityListType *>(curNode)) {
			AstNode::AstNodePtrVectorType otherEntityNodeVector = this->visit(curNode);
			// TODO: Figure out if this is the correct order (prepend or append?).
			entityNodeVector.insert(entityNodeVector.end(), otherEntityNodeVector.begin(), otherEntityNodeVector.end());
		}
	}
	return entityNodeVector;
}

/** Define a custom visitor for the declaration list visitor to make a list of declarations. */
antlrcpp::Any ParseTreeVisitorImpl::visitProgram(AntlrGrammarGenerated::TParser::ProgramContext *ctx) {
	/** Create the root node of the AST, the ProgramContext rule node. */
	AstNode * programNode = new AstNode(ctx);
	/** Set the vector of Declaration nodes as the children of the Program node. */
	AstNode::AstNodePtrVectorType declNodeVector = this->visitChildren(ctx);
	// Update the parent of each node.
	for (auto & node : declNodeVector) {
		node->parent = programNode;
	}
	programNode->children = declNodeVector;
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
					// All variable declarations will be integer types. 
					symbolTableIterator->second->type = CMINUS_NATIVE_TYPES::INT;
					// Assume we have just a single integer declaration here, can update for array afterward.
					symbolTableIterator->second->storageSize = sizeof(int);
					symbolTableIterator->second->kind = SYMBOL_RECORD_KIND::VARIABLE;
					// Declaration and definition of variables of primitive types is synonymous.
					symbolTableIterator->second->isDeclared = true;
					symbolTableIterator->second->isDefined = true;
					// Exchange pointers for association of this node with this symbol table entry.
					symbolTableIterator->second->astNode = std::make_shared<AstNode>(varDeclNode);
					varDeclNode->symbolTableRecord = symbolTableIterator->second;
				}
				else {
					std::cout << "visitVarDeclaration: WARNING: AST visit encountered Token not in symbol table yet..." << std::endl;
					// TODO: Token not found in symbol table. Insert it? ERROR?
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
	std::cout << "Saving variable from VarDeclaration in AST..." << std::endl;
	return varDeclNode;
}

/** Define a custom visitor for the FunDeclaration visitor. */
antlrcpp::Any ParseTreeVisitorImpl::visitFunDeclaration(AntlrGrammarGenerated::TParser::FunDeclarationContext * ctx) {
	AstNode * funDeclNode = new AstNode(ctx);	
	// Check if an ID exists where we expect. If so, populate the current node as a function declaration.
	if (antlrcpp::is<antlr4::tree::TerminalNode *>(ctx->children.at(1))) {
		// Save the function name (ID) for further processing.
		auto idNode = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children.at(1));
		// Save the function parameters.
		AstNode * paramListNode = this->visit(ctx->children.at(3));
		paramListNode->parent = funDeclNode;
		funDeclNode->children.push_back(paramListNode);
		// Save the function body compound statement.
		AstNode * compoundStatementNode = this->visit(ctx->children.at(5));
		compoundStatementNode->parent = funDeclNode;
		funDeclNode->children.push_back(compoundStatementNode);
		// Find the associated entry in the symbol table and update its information.
		auto symbolTableIterator = this->parser->getSymbolTable()->symbolTable.find(idNode->getSymbol()->getText());
		if (symbolTableIterator != this->parser->getSymbolTable()->symbolTable.end()) {
			// Visit the type specifier node to get the function's return type.
			std::string functionReturnType = this->visit(ctx->children.at(0));
			if (functionReturnType == "int") {
				symbolTableIterator->second->type = CMINUS_NATIVE_TYPES::INT;
				symbolTableIterator->second->returnType = CMINUS_NATIVE_TYPES::INT;
			}
			else {
				symbolTableIterator->second->type = CMINUS_NATIVE_TYPES::VOID;
				symbolTableIterator->second->returnType = CMINUS_NATIVE_TYPES::VOID;
			}
			symbolTableIterator->second->kind = SYMBOL_RECORD_KIND::FUNCTION;
			// When a C-Minus function is declared, it is also defined .
			symbolTableIterator->second->isDeclared = true;
			symbolTableIterator->second->isDefined = true;
			// Exchange pointers for association of this node with this symbol table entry.
			symbolTableIterator->second->astNode = std::make_shared<AstNode>(funDeclNode);
			funDeclNode->symbolTableRecord = symbolTableIterator->second;
			// TODO: Fill this in when we are able to parse the function 'params' node.
			//symbolTableIterator->second->numArguments = ;
		}
		else {
			std::cout << "visitFunDeclaration: WARNING: AST visit encountered Token not in symbol table yet..." << std::endl;
			// TODO: Token not found in symbol table. Insert it? ERROR?
		}
	}
	std::cout << "Saving variable from FunDeclaration in AST..." << std::endl;
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
		// Update the parent of each node.
		for (auto & node : childNodes) {
			node->parent = paramsNode;
		}
		paramsNode->children = childNodes;
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
	AstNode * paramNode = new AstNode(ctx);
	auto symbolTableIterator = this->parser->getSymbolTable()->symbolTable.begin();
	// Loop through all children and populate this AST node. 
	for (auto const & child : ctx->children) {
		// Find the ID or BRACKET nodes that define the parameter. 
		if (antlrcpp::is<antlr4::tree::TerminalNode *>(child)) {
			auto curNode = dynamic_cast<antlr4::tree::TerminalNode *>(child);
			auto nodeType = (dynamic_cast<antlr4::tree::TerminalNode *>(child))->getSymbol()->getType();
			// Save some info from the parameter ID node.
			if (nodeType == this->parser->ID) {
				// Find the associated entry in the symbol table and update its information.
				symbolTableIterator = this->parser->getSymbolTable()->symbolTable.find(curNode->getSymbol()->getText());
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
					std::cout << "visitParam: WARNING: AST visit encountered Token not in symbol table yet..." << std::endl;
					// TODO: Token not found in symbol table. Insert it? ERROR?
				}
			}
			// If we see the brackets, we know this is an array parameter.
			else if (nodeType == this->parser->LEFT_BRACKET) {
				symbolTableIterator->second->kind = SYMBOL_RECORD_KIND::ARRAY;
				// TODO: Need more context to update storage size of array parameter...
				//symbolTableIterator->second->storageSize = 4;
			}
		}
	}
	std::cout << "Saving parameter from Param in AST..." << std::endl;
	return paramNode;
}

/** Define a custom visitor for the compound statement. */
antlrcpp::Any ParseTreeVisitorImpl::visitCompoundStmt(AntlrGrammarGenerated::TParser::CompoundStmtContext * ctx) {
	AstNode * compoundStatementNode = new AstNode(ctx);
	// Build a local declarations node, whose children are variable declarations.
	AstNode * localDeclsNode = new AstNode(ctx->children.at(1)); 
	AstNode::AstNodePtrVectorType varDeclsVector = this->visit(ctx->children.at(1));
	localDeclsNode->children = varDeclsVector;
	// Update the parent of each new variable declaration node.
	for (auto & node : localDeclsNode->children) {
		node->parent = localDeclsNode;
	}
	localDeclsNode->parent = compoundStatementNode;
	compoundStatementNode->children.push_back(localDeclsNode);
	// Build a statement list node, whose children are function statements.
	AstNode * statementListNode = new AstNode(ctx->children.at(2));
	AstNode::AstNodePtrVectorType statementsVector = this->visit(ctx->children.at(2));
	statementListNode->children = statementsVector;
	// Update the parent of each new statement node.
	for (auto & node : statementListNode->children) {
		node->parent = statementListNode;
	}
	statementListNode->parent = compoundStatementNode;
	compoundStatementNode->children.push_back(statementListNode);
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
		// TODO: Prune this out later.
		return new AstNode(ctx);
	}
	// Otherwise, visit the expression and save its information.
	else {
		return this->visit(ctx->children.at(0));
	}
}


antlrcpp::Any ParseTreeVisitorImpl::visitSelectionStmt(AntlrGrammarGenerated::TParser::SelectionStmtContext * ctx) {
	// Check number of children to see if an ELSE is used.
	if (ctx->children.size() == 5) {
		// Save expression and statement as children.
		std::cout << "visitSelectionStatement: Encountered if-statement." << std::endl;
	}
	// Process an ELSE statement also.
	else {
		// Save expression and 2 statements as children.
		std::cout << "visitSelectionStatement: Encountered if-else-statement." << std::endl;
	}
	// TODO: Fix this up.
	return new AstNode(ctx);
}

antlrcpp::Any ParseTreeVisitorImpl::visitIterationStmt(AntlrGrammarGenerated::TParser::IterationStmtContext * ctx) {
	// Save expression and statement.
	return new AstNode(ctx);
}

antlrcpp::Any ParseTreeVisitorImpl::visitReturnStmt(AntlrGrammarGenerated::TParser::ReturnStmtContext * ctx) {
	// Check if there is any return value.
	if (ctx->children.size() == 3) {
		// Save return expression as child.
		std::cout << "visitReturnStatement: Encountered return expression." << std::endl;
	}
	else {
		std::cout << "visitReturnStatement: Encountered return." << std::endl;
	}
	// TODO: Fix this up.
	return new AstNode(ctx);
}

antlrcpp::Any ParseTreeVisitorImpl::visitExpression(AntlrGrammarGenerated::TParser::ExpressionContext * ctx) {
	// TODO: Fill this in.
	return new AstNode(ctx);
}







