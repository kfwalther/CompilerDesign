#ifndef ASTVISITORIMPL_H
#define ASTVISITORIMPL_H

/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: AstVisitorImpl
 */

#include "AstVisitor.h"

 // Forward declarations.
struct Compiler;
struct SymbolRecord;

/** Define the AstVisitorImpl class, which implements the abstract methods defined in AstVisitor. */
struct AstVisitorImpl : public AstVisitor {
	/** Alias some commonly used types for convenience. */
	typedef std::shared_ptr<SymbolRecord> SymbolRecordPtrType;

	/** Define a constructor. */
	AstVisitorImpl(Compiler * const & compiler);
	
	/** Define a function to visit a specific node. */
	void visit(AstNode * ctx);
	/** Define a function to visit all children in this node. */
	void visitChildren(AstNode * ctx);

	/** Define a helper function to verify the operand types for various mathematical operations. */
	void verifyMathOperandTypes(AstNode * ctx);
	/** Define a helper function to verify the operands are declared and assigned. */
	void verifyOperandUsability(AstNode * ctx);
	/** Define a helper function to get the function declaration which defines the current scope. */
	SymbolRecordPtrType const & getCurrentScopeFunctionDeclaration(AstNode * ctx);

	void visitProgram(AstNode * ctx);
	void visitDeclarationList(AstNode * ctx);
	void visitVarDeclaration(AstNode * ctx);
	void visitFunDeclaration(AstNode * ctx);
	void visitTypeSpecifier(AstNode * ctx);
	void visitParams(AstNode * ctx);
	void visitParamList(AstNode * ctx);
	void visitParam(AstNode * ctx);
	void visitCompoundStmt(AstNode * ctx);
	void visitLocalDeclaration(AstNode * ctx);
	void visitStatementList(AstNode * ctx);

	void visitExpressionStmt(AstNode * ctx);
	void visitSelectionStmt(AstNode * ctx);
	void visitIterationStmt(AstNode * ctx);
	void visitReturnStmt(AstNode * ctx);

	void visitExpression(AstNode * ctx);
	void visitVar(AstNode * ctx);
	void visitSimpleExpression(AstNode * ctx);
	void visitAdditiveExpression(AstNode * ctx);
	void visitTerm(AstNode * ctx);
	void visitFactor(AstNode * ctx);
	void visitCall(AstNode * ctx);
	void visitArgs(AstNode * ctx);
	void visitArgList(AstNode * ctx);

private:
	Compiler * compiler;
};


#endif /* ASTVISITORIMPL_H */