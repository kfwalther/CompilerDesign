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

	virtual void visitProgram(AstNode * ctx) = 0;
	//virtual void visitDeclarationList(AstNode * ctx) = 0;
	//virtual void visitVarDeclaration(AstNode * ctx) = 0;
	virtual void visitFunDeclaration(AstNode * ctx) = 0;
	//virtual void visitTypeSpecifier(AstNode * ctx) = 0;
	//virtual void visitParams(AstNode * ctx) = 0;
	//virtual void visitParamList(AstNode * ctx) = 0;
	//virtual void visitParam(AstNode * ctx) = 0;
	//virtual void visitCompoundStmt(AstNode * ctx) = 0;
	//virtual void visitLocalDeclaration(AstNode * ctx) = 0;
	//virtual void visitStatementList(AstNode * ctx) = 0;
	//virtual void visitExpressionStmt(AstNode * ctx) = 0;
	//virtual void visitSelectionStmt(AstNode * ctx) = 0;
	//virtual void visitIterationStmt(AstNode * ctx) = 0;
	//virtual void visitReturnStmt(AstNode * ctx) = 0;
	//virtual void visitExpression(AstNode * ctx) = 0;
	//virtual void visitVar(AstNode * ctx) = 0;
	//virtual void visitSimpleExpression(AstNode * ctx) = 0;
	//virtual void visitAdditiveExpression(AstNode * ctx) = 0;
	//virtual void visitTerm(AstNode * ctx) = 0;
	//virtual void visitFactor(AstNode * ctx) = 0;
	//virtual void visitCall(AstNode * ctx) = 0;
	//virtual void visitArgs(AstNode * ctx) = 0;
	//virtual void visitArgList(AstNode * ctx) = 0;

private:

};


#endif /* ASTVISITOR_H */