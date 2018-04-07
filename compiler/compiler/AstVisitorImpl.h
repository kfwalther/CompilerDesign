#ifndef ASTVISITORIMPL_H
#define ASTVISITORIMPL_H

/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: AstVisitorImpl
 */

#include "AstVisitor.h"

 /** Define the AstVisitorImpl class, which implements the abstract methods defined in AstVisitor. */
struct AstVisitorImpl : public AstVisitor {
	/** Define a constructor. */
	AstVisitorImpl();
	
	/** Define a function to visit a specific node. */
	antlrcpp::Any visit(AstNode * ctx);
	/** Define a function to visit all children in this node. */
	antlrcpp::Any visitChildren(AstNode * ctx);


	antlrcpp::Any visitProgram(AstNode * ctx);
	antlrcpp::Any visitDeclarationList(AstNode * ctx);
	// antlrcpp::Any visitVarDeclaration(AstNode * ctx) = 0;
	// antlrcpp::Any visitFunDeclaration(AstNode * ctx) = 0;
	// antlrcpp::Any visitTypeSpecifier(AstNode * ctx) = 0;
	// antlrcpp::Any visitParams(AstNode * ctx) = 0;
	// antlrcpp::Any visitParamList(AstNode * ctx) = 0;
	// antlrcpp::Any visitParam(AstNode * ctx) = 0;
	// antlrcpp::Any visitCompoundStmt(AstNode * ctx) = 0;
	// antlrcpp::Any visitLocalDeclaration(AstNode * ctx) = 0;
	// antlrcpp::Any visitStatementList(AstNode * ctx) = 0;

	// antlrcpp::Any visitExpressionStmt(AstNode * ctx) = 0;
	// antlrcpp::Any visitSelectionStmt(AstNode * ctx) = 0;
	// antlrcpp::Any visitIterationStmt(AstNode * ctx) = 0;
	// antlrcpp::Any visitReturnStmt(AstNode * ctx) = 0;

	// antlrcpp::Any visitExpression(AstNode * ctx) = 0;
	// antlrcpp::Any visitVar(AstNode * ctx) = 0;
	// antlrcpp::Any visitSimpleExpression(AstNode * ctx) = 0;
	// antlrcpp::Any visitAdditiveExpression(AstNode * ctx) = 0;
	// antlrcpp::Any visitTerm(AstNode * ctx) = 0;
	// antlrcpp::Any visitFactor(AstNode * ctx) = 0;
	// antlrcpp::Any visitCall(AstNode * ctx) = 0;
	// antlrcpp::Any visitArgs(AstNode * ctx) = 0;
	// antlrcpp::Any visitArgList(AstNode * ctx) = 0;

private:
	AntlrGrammarGenerated::TParser * parser;
};


#endif /* ASTVISITORIMPL_H */