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
	AstNode * valDeclNode = new AstNode(ctx);
	
	// Loop through all children and save AST node. 
	for (auto const & child : ctx->children) {
		// Find the ID/NUM nodes that declare the variable. 
		if (antlrcpp::is<antlr4::tree::TerminalNode *>(child)) {
			auto curNode = dynamic_cast<antlr4::tree::TerminalNode *>(child);
			auto nodeType = (dynamic_cast<antlr4::tree::TerminalNode *>(child))->getSymbol()->getType();
			if ((nodeType == this->parser->NUM) || (nodeType == this->parser->ID) || 
					(nodeType == this->parser->LEFT_BRACKET) || (nodeType == this->parser->RIGHT_BRACKET)) {
				// Create new AST node and save it. 
				valDeclNode->children.push_back(new AstNode(child, valDeclNode));
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
					symbolTableIterator->second->astNode = std::make_shared<AstNode>(valDeclNode);
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
	return valDeclNode;
}

/** Define a custom visitor for the FunDeclaration visitor. */
antlrcpp::Any ParseTreeVisitorImpl::visitFunDeclaration(AntlrGrammarGenerated::TParser::FunDeclarationContext * ctx) {
	AstNode * funDeclNode = new AstNode(ctx->children.at(1));
	// Visit the type specifier node to get the return type of this function. */
	std::string functionReturnType = this->visit(ctx->children.at(0));
	// Save the function name (ID) as the current AST node.
	if (antlrcpp::is<antlr4::tree::TerminalNode *>(ctx->children.at(1))) {
		auto idNode = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children.at(1));
		// Save the function parameters.
		funDeclNode->children.push_back(new AstNode(ctx->children.at(3), funDeclNode));
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
