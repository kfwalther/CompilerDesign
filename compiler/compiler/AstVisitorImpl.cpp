/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: AstVisitorImpl
 */

#include "AstVisitorImpl.h"

/** Define the default constructor for the Parse Tree Listener. */
// TODO: Finish filling out these visitors.
AstVisitorImpl::AstVisitorImpl() {
	return;
}

/** Define a function to visit a specific node. */
void AstVisitorImpl::visit(AstNode * ctx) {
	switch (ctx->ruleType) {
		case CMINUS_RULE_TYPE::RuleProgram: this->visitProgram(ctx); return;
		case CMINUS_RULE_TYPE::RuleDeclarationList: this->visitDeclarationList(ctx); return;
		case CMINUS_RULE_TYPE::RuleVarDeclaration: this->visitVarDeclaration(ctx); return; 
		case CMINUS_RULE_TYPE::RuleTypeSpecifier: this->visitTypeSpecifier(ctx); return;
		case CMINUS_RULE_TYPE::RuleFunDeclaration: this->visitFunDeclaration(ctx); return;
		case CMINUS_RULE_TYPE::RuleParams: this->visitParams(ctx); return;
		case CMINUS_RULE_TYPE::RuleParamList: this->visitParamList(ctx); return;
		case CMINUS_RULE_TYPE::RuleParam: this->visitParam(ctx); return;
		case CMINUS_RULE_TYPE::RuleCompoundStmt: this->visitCompoundStmt(ctx); return;
		case CMINUS_RULE_TYPE::RuleLocalDeclaration: this->visitLocalDeclaration(ctx); return; 
		case CMINUS_RULE_TYPE::RuleStatementList: this->visitStatementList(ctx); return;
		case CMINUS_RULE_TYPE::RuleExpressionStmt: this->visitExpressionStmt(ctx); return;
		case CMINUS_RULE_TYPE::RuleSelectionStmt: this->visitSelectionStmt(ctx); return;
		case CMINUS_RULE_TYPE::RuleIterationStmt: this->visitIterationStmt(ctx); return;
		case CMINUS_RULE_TYPE::RuleReturnStmt: this->visitReturnStmt(ctx); return;
		case CMINUS_RULE_TYPE::RuleExpression: this->visitExpression(ctx); return;
		case CMINUS_RULE_TYPE::RuleVar: this->visitVar(ctx); return;
		case CMINUS_RULE_TYPE::RuleSimpleExpression: this->visitSimpleExpression(ctx); return;
		case CMINUS_RULE_TYPE::RuleAdditiveExpression: this->visitAdditiveExpression(ctx); return;
		case CMINUS_RULE_TYPE::RuleTerm: this->visitTerm(ctx); return;
		case CMINUS_RULE_TYPE::RuleFactor: this->visitFactor(ctx); return;
		case CMINUS_RULE_TYPE::RuleCall: this->visitCall(ctx); return;
		case CMINUS_RULE_TYPE::RuleArgs: this->visitArgs(ctx); return;
		case CMINUS_RULE_TYPE::RuleArgList: this->visitArgList(ctx); return;
	}
}

/** Define a function to visit all children in this node. */
void AstVisitorImpl::visitChildren(AstNode * ctx) {
	for (auto & child : ctx->children) {
		this->visit(child);
	}
	return;
}

/** Define a custom visitor for the AST root node. */
void AstVisitorImpl::visitProgram(AstNode * ctx) {
	std::cout << "Walking the AST for semantic analysis!" << std::endl;
	if (ctx->children.back()->symbolTableRecord->text != "main") {
		// TODO: Does this case need a new error code?
		std::cerr << "ERROR: Main is not the last declaration!" << std::endl << std::endl;
	}
	this->visitChildren(ctx);
	return;
}

void AstVisitorImpl::visitDeclarationList(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitVarDeclaration(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitFunDeclaration(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitTypeSpecifier(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitParams(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitParamList(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitParam(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitCompoundStmt(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitLocalDeclaration(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitStatementList(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitExpressionStmt(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitSelectionStmt(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitIterationStmt(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitReturnStmt(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitExpression(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitVar(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitSimpleExpression(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitAdditiveExpression(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitTerm(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitFactor(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitCall(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitArgs(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}
void AstVisitorImpl::visitArgList(AstNode * ctx) {
	this->visitChildren(ctx);
	return;
}




