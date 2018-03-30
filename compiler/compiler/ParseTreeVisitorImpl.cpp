/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "ParseTreeVisitorImpl.h"

/** Define the default constructor for the Parse Tree Listener. */
ParseTreeVisitorImpl::ParseTreeVisitorImpl(AntlrGrammarGenerated::TParser * const parser) : parser(parser) {
	return;
}

/** Define a custom visitor for the declaration list visitor to make a list of declarations. */
antlrcpp::Any ParseTreeVisitorImpl::visitProgram(AntlrGrammarGenerated::TParser::ProgramContext *ctx) {
	/** Create the root node of the AST, the ProgramContext rule node. */
	AstNode * programNode = new AstNode(ctx);
	/** Set the vector of Declaration nodes as the children of the Program node. */
	std::vector<AstNode *> declNodeVector = this->visitChildren(ctx);
	// Update the parent of each node.
	for (auto & node : declNodeVector) {
		node->parent = programNode;
	}
	programNode->children = declNodeVector;
	return programNode;
}

/** Define a custom visitor for the declaration list visitor to make a list of declarations. */
antlrcpp::Any ParseTreeVisitorImpl::visitDeclarationList(AntlrGrammarGenerated::TParser::DeclarationListContext * ctx) {
	// Declare a vector of AstNodes to return.
	std::vector<AstNode *> declNodeVector;
	// Loop through all nodes in the declaration list (either Declaration OR DeclarationList Declaration).
	for (auto const & curNode : ctx->children) {
		// If the current node is just a declaration, add it to the declNodeVector.
		if (antlrcpp::is<AntlrGrammarGenerated::TParser::DeclarationContext *>(curNode)) {
			// Visit the child first to create a node for specific type of declaration (function or variable decl).
			declNodeVector.push_back(this->visit(curNode));
		}
		// Append the declNodes from the nested DeclarationList to declNodeVector.
		if (antlrcpp::is<AntlrGrammarGenerated::TParser::DeclarationListContext *>(curNode)) {
			std::vector<AstNode *> otherDeclNodeVector = this->visit(curNode);
			// TODO: Figure out if this is the correct order (prepend or append?).
			declNodeVector.insert(declNodeVector.end(), otherDeclNodeVector.begin(), otherDeclNodeVector.end());
		}
	}
	// Return the vector of declaration nodes.
	return declNodeVector;
}

/** Define a custom visitor for the VarDeclaration visitor. */
antlrcpp::Any ParseTreeVisitorImpl::visitVarDeclaration(AntlrGrammarGenerated::TParser::VarDeclarationContext * ctx) {
	AstNode * varDeclNode = new AstNode(ctx);
	
	// Loop through all children and save AST node. 
	for (auto const & child : ctx->children) {
		// Find the ID/NUM nodes that declare the variable. 
		if (antlrcpp::is<antlr4::tree::TerminalNode *>(child)) {
			auto curNode = dynamic_cast<antlr4::tree::TerminalNode *>(child);
			auto nodeType = (dynamic_cast<antlr4::tree::TerminalNode *>(child))->getSymbol()->getType();
			if ((nodeType == this->parser->NUM) || (nodeType == this->parser->ID) || 
					(nodeType == this->parser->LEFT_BRACKET) || (nodeType == this->parser->RIGHT_BRACKET)) {
				// Create new AST node and save it. 
				// TODO: Don't need to save all these children. Get info from them and make the current node the leaf.
				varDeclNode->children.push_back(new AstNode(child, varDeclNode));
			}
			// Extra processing if we have the ID.
			if (nodeType == this->parser->ID) {
				// Find the associated entry in the symbol table and update its information.
				auto symbolTableIterator = this->parser->getSymbolTable()->symbolTable.find(curNode->getSymbol()->getText());
				if (symbolTableIterator != this->parser->getSymbolTable()->symbolTable.end()) {
					// All variable declarations will be integer types. 
					symbolTableIterator->second->type = CMINUS_NATIVE_TYPES::INT;
					symbolTableIterator->second->storageSize = 4;
					// Declaration and definition of variables of primitive types is synonymous.
					symbolTableIterator->second->isDeclared = true;
					symbolTableIterator->second->isDefined = true;
					symbolTableIterator->second->astNode = std::make_shared<AstNode>(varDeclNode);
					// TODO: Populate SYMBOL_RECORD_KIND here.
					//symbolTableIterator->second->kind = SYMBOL_RECORD_KIND::VARIABLE;
					//symbolTableIterator->second->kind = SYMBOL_RECORD_KIND::ARRAY;
				}
				else {
					std::cout << "visitVarDeclaration: WARNING: AST visit encountered Token not in symbol table yet..." << std::endl;
					// TODO: Token not found in symbol table. Insert it? ERROR?
				}
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
		// Visit the type specifier node to get the return type of this function. */
		std::string functionReturnType = this->visit(ctx->children.at(0));
		// Save the function name (ID) for further processing.
		auto idNode = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children.at(1));
		funDeclNode->children.push_back(new AstNode(ctx->children.at(1), funDeclNode));
		// Save the function parameters.
		AstNode * paramListNode = this->visit(ctx->children.at(3));
		paramListNode->parent = funDeclNode;
		funDeclNode->children.push_back(paramListNode);
		// Save the function body.
		funDeclNode->children.push_back(new AstNode(ctx->children.at(5), funDeclNode));
		// Find the associated entry in the symbol table and update its information.
		auto symbolTableIterator = this->parser->getSymbolTable()->symbolTable.find(idNode->getSymbol()->getText());
		if (symbolTableIterator != this->parser->getSymbolTable()->symbolTable.end()) {
			symbolTableIterator->second->astNode = std::make_shared<AstNode>(funDeclNode);
			// Populate the return type of this function.
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
		std::vector<AstNode *> childNodes = this->visit(ctx->children.at(0));
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
	// Declare a vector of AstNodes to return.
	std::vector<AstNode *> paramNodeVector;
	// Loop through all nodes in the param list (param | paramList COMMA param).
	for (auto const & curNode : ctx->children) {
		// If the current node is just a param, add it to the paramNodeVector.
		if (antlrcpp::is<AntlrGrammarGenerated::TParser::ParamContext *>(curNode)) {
			// Visit the child first to create a node for specific parameter.
			paramNodeVector.push_back(this->visit(curNode));
		}
		// Append the paramNodes from the nested ParamList to paramNodeVector.
		if (antlrcpp::is<AntlrGrammarGenerated::TParser::ParamListContext *>(curNode)) {
			std::vector<AstNode *> otherParamNodeVector = this->visit(curNode);
			// TODO: Figure out if this is the correct order (prepend or append?).
			paramNodeVector.insert(paramNodeVector.end(), otherParamNodeVector.begin(), otherParamNodeVector.end());
		}
	}
	// Return the vector of param nodes.
	return paramNodeVector;
}

/** Define a custom visitor for the params node. */
antlrcpp::Any ParseTreeVisitorImpl::visitParam(AntlrGrammarGenerated::TParser::ParamContext * ctx) {
	AstNode * paramNode = new AstNode(ctx);
	bool gotLeftBracket = false;
	bool gotRightBracket = false;
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
					symbolTableIterator->second->storageSize = 4;
					// Declaration and definition of parameters of primitive types is synonymous.
					symbolTableIterator->second->isDeclared = true;
					symbolTableIterator->second->isDefined = true;
					symbolTableIterator->second->astNode = std::make_shared<AstNode>(paramNode);
				} else {
					std::cout << "visitParam: WARNING: AST visit encountered Token not in symbol table yet..." << std::endl;
					// TODO: Token not found in symbol table. Insert it? ERROR?
				}
			}
			// Keep track of brackets if we see them.
			else if (nodeType == this->parser->LEFT_BRACKET) {
				gotLeftBracket = true;
			} else if (nodeType == this->parser->RIGHT_BRACKET) {
				gotRightBracket = true;
			}
		}
	}
	// Check if this was an array parameter.
	if (gotLeftBracket && gotRightBracket) {
		symbolTableIterator->second->kind = SYMBOL_RECORD_KIND::ARRAY;
	} else if (!gotLeftBracket && !gotRightBracket) {
		symbolTableIterator->second->kind = SYMBOL_RECORD_KIND::VARIABLE;
	} else {
		// Should not hit this case b/c it wouldn't have parsed. 
		std::cerr << "visitParam: WARNING: Missing bracket in array function paramter!" << std::endl;
	}
	std::cout << "Saving parameter from Param in AST..." << std::endl;
	return paramNode;
}

/** Define a custom visitor for the compound statement. */
antlrcpp::Any ParseTreeVisitorImpl::visitCompountStmt(AntlrGrammarGenerated::TParser::CompoundStmtContext * ctx) {
	AstNode * compoundStatementNode = new AstNode(ctx);
	//TOOD: Fix this!
	// Build two vectors, declarations and statements, to hold in this node.
	std::vector<AstNode *> declarationChildNodes = this->visit(ctx->children.at(1));
	// Update the parent of each node.
	for (auto & node : declarationChildNodes) {
		node->parent = compoundStatementNode;
	}
	//compoundStatementNode->children = declarationChildNodes;
	std::vector<AstNode *> statementChildNodes = this->visit(ctx->children.at(2));
	// Update the parent of each node.
	for (auto & node : statementChildNodes) {
		node->parent = compoundStatementNode;
	}
	//compoundStatementNode->children = statementChildNodes;
	return compoundStatementNode;
}














