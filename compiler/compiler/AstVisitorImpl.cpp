/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: AstVisitorImpl
 */

#include "AstVisitorImpl.h"
#include "ErrorHandler.h"

/** Define the default constructor for the Parse Tree Listener. */
AstVisitorImpl::AstVisitorImpl(Compiler * const & compiler) : compiler(compiler) {
	return;
}

/** Define a helper function to verify the operand types for various mathematical operations. */
void AstVisitorImpl::verifyMathOperandTypes(AstNode * ctx) {
	// Ensure the operands for the mathematical operation are INT types.
	if ((ctx->children.front()->evaluatedType == CMINUS_NATIVE_TYPES::INT)
			&& (ctx->children.back()->evaluatedType == CMINUS_NATIVE_TYPES::INT)) {
		ctx->evaluatedType = CMINUS_NATIVE_TYPES::INT;
	} else {
		// If either operand is not an INT type, print an error.
		this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::TYPE_MISMATCH,
				ctx->symbolTableRecord->token->getLine(), ("Illegal type used in mathematical operation."));
	}
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
}

/** Define a custom visitor for the AST root node. */
void AstVisitorImpl::visitProgram(AstNode * ctx) {
	std::cout << "Walking the AST for semantic analysis!" << std::endl;
	// Check to ensure the main() function is the last declaration in the program. (Rule enforced on pg. 493 of textbook.)
	if (ctx->children.back()->symbolTableRecord->text != "main") {
		this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::INVALID_SYNTAX,
				ctx->children.back()->symbolTableRecord->token->getLine(), ("Main is not the last declaration in the program! \
						Consider restructuring your program to have the main function as the last declaration."));
	}
	this->visitChildren(ctx);
}

void AstVisitorImpl::visitDeclarationList(AstNode * ctx) {
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitVarDeclaration(AstNode * ctx) {
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitFunDeclaration(AstNode * ctx) {
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitTypeSpecifier(AstNode * ctx) {
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitParams(AstNode * ctx) {
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitParamList(AstNode * ctx) {
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitParam(AstNode * ctx) {
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitCompoundStmt(AstNode * ctx) {
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitLocalDeclaration(AstNode * ctx) {
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitStatementList(AstNode * ctx) {
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitExpressionStmt(AstNode * ctx) {
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitSelectionStmt(AstNode * ctx) {
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitIterationStmt(AstNode * ctx) {
	this->visitChildren(ctx);
}

void AstVisitorImpl::visitReturnStmt(AstNode * ctx) {
	this->visitChildren(ctx);
	// Check if there was a return value.
	if (!ctx->children.empty()) {
		// If we return a value, it must be an INT type.
		if (ctx->parent->parent->parent->symbolTableRecord->returnType != CMINUS_NATIVE_TYPES::INT) {
			// Print an error indicating we are returning a value, but return type in function declaration is VOID.
			this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::NO_RETURN_VAL,
					ctx->children.front()->symbolTableRecord->token->getLine(), ("Declaration of function " 
							+ ctx->parent->parent->parent->symbolTableRecord->text + " specified VOID return type, but the function returns a value."));
		} 
		if (ctx->children.front()->evaluatedType != CMINUS_NATIVE_TYPES::INT) {
			// Print an error indicating invalid return type.
			this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::INVALID_TYPE,
					ctx->children.front()->symbolTableRecord->token->getLine(), ("Return value of function "
							+ ctx->parent->parent->parent->symbolTableRecord->text + " is type VOID or UNKNOWN, expected INT."));
		}
	} else {
		// Check for the case in which we expected a return value, but did not have one.
		if (ctx->parent->parent->parent->symbolTableRecord->returnType == CMINUS_NATIVE_TYPES::INT) {
			// Print an error indicating a missing return type.
			this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::MISSING_RETURN_VAL,
					ctx->token->getLine(), ("Return value of type INT expected for function "
							+ ctx->parent->parent->parent->symbolTableRecord->text + "."));
		}
	}
}

void AstVisitorImpl::visitExpression(AstNode * ctx) {
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitVar(AstNode * ctx) {
	this->visitChildren(ctx);
}

void AstVisitorImpl::visitSimpleExpression(AstNode * ctx) {
	this->visitChildren(ctx);
	// Ensure the operands for the relative operation are INT types.
	this->verifyMathOperandTypes(ctx);
}

// TODO: Add checks to these to ensure operands can be used (have been declared and assigned).
void AstVisitorImpl::visitAdditiveExpression(AstNode * ctx) {
	this->visitChildren(ctx);
	// Ensure the operands for the addition/subtraction operation are INT types.
	this->verifyMathOperandTypes(ctx);
}

void AstVisitorImpl::visitTerm(AstNode * ctx) {
	this->visitChildren(ctx);
	// Ensure the operands for the mult/division operation are INT types.
	this->verifyMathOperandTypes(ctx);
}

void AstVisitorImpl::visitFactor(AstNode * ctx) {
	this->visitChildren(ctx);
}

void AstVisitorImpl::visitCall(AstNode * ctx) {
	// Look up the function name, and get its return type.
	ctx->evaluatedType = ctx->symbolTableRecord->returnType;
	this->visitChildren(ctx);
}

void AstVisitorImpl::visitArgs(AstNode * ctx) {
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitArgList(AstNode * ctx) {
	this->visitChildren(ctx);
}




