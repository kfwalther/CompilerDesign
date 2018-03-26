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
	std::vector<AstNode::AstNodePtrType> declNodeVector = visitChildren(ctx);
	programNode->children = declNodeVector;
	return programNode;
}

/** Define a custom visitor for the declaration list visitor to make a list of declarations. */
antlrcpp::Any ParseTreeVisitorImpl::visitDeclarationList(AntlrGrammarGenerated::TParser::DeclarationListContext * ctx) {
	// Create a vector of AstNodes to return.
	std::vector<AstNode::AstNodePtrType> declNodeVector;
	// Loop through all nodes in the declaration list (either Declaration OR DeclarationList Declaration).
	for (auto const & curNode : ctx->children) {
		// Add the current node to the declNodeVector.
		if (antlrcpp::is<AntlrGrammarGenerated::TParser::DeclarationContext *>(curNode)) {
			declNodeVector.push_back(std::make_shared<AstNode>(curNode));
		}
		// Prepend the declNodes from the nested DeclarationList to declNodeVector.
		if (antlrcpp::is<AntlrGrammarGenerated::TParser::DeclarationListContext *>(curNode)) {
			std::vector<AstNode::AstNodePtrType> otherDeclNodeVector = visit(curNode);
			declNodeVector.insert(declNodeVector.end(), otherDeclNodeVector.begin(), otherDeclNodeVector.end());
		}
	}
	// Return the vector of declaration nodes.
	return declNodeVector;
}


/** Define a custom visitor for the VarDeclaration visitor. */
antlrcpp::Any ParseTreeVisitorImpl::visitVarDeclaration(AntlrGrammarGenerated::TParser::VarDeclarationContext * ctx) {
	AstNode * tempAstNode = new AstNode(ctx);
	/** Handle the typeSpecifier here. We want to save the type of the variable. */
	auto variableType = this->visit(ctx->children.at(0));
	//TODO: Save the variable type here.
	/** Loop through all children and save AST node. */
	for (auto const & child : ctx->children) {
		/** Find the ID/NUM nodes that declare the variable. */
		if (antlrcpp::is<antlr4::tree::TerminalNode *>(child)) {
			auto nodeType = (dynamic_cast<antlr4::tree::TerminalNode *>(child))->getSymbol()->getType();
			if ((nodeType == this->parser->NUM) || (nodeType == this->parser->ID) || 
					(nodeType == this->parser->LEFT_BRACKET) || (nodeType == this->parser->RIGHT_BRACKET)) {
				/** Create new AST node and save it. */
				tempAstNode->children.push_back(std::make_shared<AstNode>(child));
			}
		}
	}
	std::cout << "Saving variable from VarDeclaration in AST..." << std::endl;
	return tempAstNode;
}

/** Define a custom visitor for the FunDeclaration visitor. */
antlrcpp::Any ParseTreeVisitorImpl::visitFunDeclaration(AntlrGrammarGenerated::TParser::FunDeclarationContext * ctx) {
	/** Handle the typeSpecifier here. We want to save the type of the variable. */
	auto variableType = this->visit(ctx->children.at(0));
	//TODO: Save the variable type here.
	// Save the function name (ID) as the current AST node.
	AstNode * funDeclNode = new AstNode(ctx->children.at(1));
	//TODO: Define constructors to save data from these casts.
	// Save the function parameters.
	funDeclNode->children.push_back(std::make_shared<AstNode>(ctx->children.at(3)));
	// Save the function body.
	funDeclNode->children.push_back(std::make_shared<AstNode>(ctx->children.at(5)));

	std::cout << "Saving variable from FunDeclaration in AST..." << std::endl;
	return funDeclNode;
}


