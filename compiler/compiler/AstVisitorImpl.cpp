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
antlrcpp::Any AstVisitorImpl::visit(AstNode * ctx) {
	switch (ctx->ruleType) {
	case CMINUS_RULE_TYPE::RuleProgram: return this->visitProgram(ctx); 
	case CMINUS_RULE_TYPE::RuleDeclarationList: return this->visitDeclarationList(ctx); 
		//case CMINUS_RULE_TYPE::RuleDeclaration: return this->visitProgram(ctx);  
		//case CMINUS_RULE_TYPE::RuleVarDeclaration: return this->visitProgram(ctx);  
		//case CMINUS_RULE_TYPE::RuleTypeSpecifier: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleFunDeclaration: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleParams: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleParamList: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleParam: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleCompoundStmt: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleLocalDeclaration: return this->visitProgram(ctx);  
		//case CMINUS_RULE_TYPE::RuleStatementList: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleStatement: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleExpressionStmt: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleSelectionStmt: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleIterationStmt: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleReturnStmt: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleExpression: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleVar: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleSimpleExpression: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleRelop: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleAdditiveExpression: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleAddop: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleTerm: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleMulop: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleFactor: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleCall: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleArgs: return this->visitProgram(ctx); 
		//case CMINUS_RULE_TYPE::RuleArgList: return this->visitProgram(ctx); 
	}
}

/** Define a function to visit all children in this node. */
antlrcpp::Any AstVisitorImpl::visitChildren(AstNode * ctx) {
	for (auto & child : ctx->children) {
		return this->visit(child);
	}
}

/** Define a custom visitor for the AST root node. */
antlrcpp::Any AstVisitorImpl::visitProgram(AstNode * ctx) {
	//this->visitChildren(ctx);
	std::cout << "Walking the AST for semantic analysis!" << std::endl;
	if (ctx->children.back()->symbolTableRecord->token->getText() != "main") {
		// TODO: Does this case need a new error code?
		std::cerr << "ERROR: Main is not the last declaration!" << std::endl << std::endl;
	}
	return ctx;
}

/** Define a custom visitor for the AST root node. */
antlrcpp::Any AstVisitorImpl::visitDeclarationList(AstNode * ctx) {
	return ctx;
}



