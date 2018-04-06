/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: AstVisitorImpl
 */

#include "AstVisitorImpl.h"

/** Define the default constructor for the Parse Tree Listener. */
AstVisitorImpl::AstVisitorImpl() {
	return;
}

/** Define a custom visitor for the AST root node. */
antlrcpp::Any AstVisitorImpl::visitProgram(AstNode * ctx) {
	std::cout << "Walking the AST for semantic analysis!" << std::endl;
	// TOOD: Finish filling out these visitors.
	return new AstNode(ctx);
}




