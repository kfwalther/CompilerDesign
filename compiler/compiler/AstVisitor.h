#ifndef ASTVISITOR_H
#define ASTVISITOR_H

/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: AstVisitor
 */

#include "TParserBaseVisitor.h"
#include "AstNode.h"

 /** Define the AstVisitor, an abstract class. */
struct AstVisitor {

	virtual antlrcpp::Any visitProgram(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitDeclarationList(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitVarDeclaration(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitFunDeclaration(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitTypeSpecifier(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitParams(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitParamList(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitParam(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitCompoundStmt(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitLocalDeclaration(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitStatementList(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitExpressionStmt(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitSelectionStmt(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitIterationStmt(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitReturnStmt(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitExpression(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitVar(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitSimpleExpression(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitAdditiveExpression(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitTerm(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitFactor(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitCall(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitArgs(AstNode * ctx) = 0;
	//virtual antlrcpp::Any visitArgList(AstNode * ctx) = 0;

private:

};


#endif /* ASTVISITOR_H */