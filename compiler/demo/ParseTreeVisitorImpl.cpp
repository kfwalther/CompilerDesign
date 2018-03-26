/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "ParseTreeVisitorImpl.h"
#include "SymbolTable.h"
#include "AstTreeNode.h"

/** Define the default constructor for the Parse Tree Listener. */
ParseTreeVisitorImpl::ParseTreeVisitorImpl(antlrcpptest::TParser * const parser) : parser(parser) {
	return;
}

/** Define a custom visitor for the VarDeclaration visitor. */
antlrcpp::Any ParseTreeVisitorImpl::visitVarDeclaration(antlrcpptest::TParser::VarDeclarationContext * ctx) {
	std::vector<AstTreeNode *> astTreeNodeVector;
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
				astTreeNodeVector.push_back(dynamic_cast<AstTreeNode *>(child));
			}
		}
	}
	AstTreeNode * tempAstNode = new AstTreeNode("VarDecl");
	tempAstNode->children = astTreeNodeVector;
	std::cout << "Saving variable from VarDeclaration in AST..." << std::endl;
	return tempAstNode;
	//return antlrcpp::Any::Any<AstTreeNode>(new AstTreeNode("FunDeclaration"));
	//return dynamic_cast<antlr4::tree::ParseTree *>(astTreeNodeVector.at(0));
}


/** Define a custom visitor for the VarDeclaration visitor. */
antlrcpp::Any ParseTreeVisitorImpl::visitFunDeclaration(antlrcpptest::TParser::FunDeclarationContext * ctx) {
	std::vector<AstTreeNode *> astTreeNodeVector;
	/** Handle the typeSpecifier here. We want to save the type of the variable. */
	auto variableType = this->visit(ctx->children.at(0));
	//TODO: Save the variable type here.
	// Save the ID as the current AST node.
	//TODO: Define constructors to save data from these casts.
	AstTreeNode * curFunctionName = dynamic_cast<AstTreeNode *>(ctx->children.at(1));
	// Save the function parameters.
	curFunctionName->children.push_back(dynamic_cast<AstTreeNode *>(ctx->children.at(3)));
	// Save the function body.
	curFunctionName->children.push_back(dynamic_cast<AstTreeNode *>(ctx->children.at(5)));

	std::cout << "Saving variable from FunDeclaration in AST..." << std::endl;

	return curFunctionName;
	//return dynamic_cast<antlr4::tree::ParseTree *>(curFunctionName);
}


