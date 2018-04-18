/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: AstVisitorImpl
 */

#include "AstVisitorImpl.h"
#include "ErrorHandler.h"
#include "LanguageDefinitions.h"
#include "AstNode.h"
#include "SymbolTable.h"
#include "SymbolTableManager.h"
#include "Scope.h"
#include "Compiler.h"

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

/** Define a helper function to verify the operands are declared. */
void AstVisitorImpl::verifyOperandUsability(AstNode * ctx) {
	if (ctx->children.front()->symbolTableRecord != NULL) {
		// Check to ensure that any variable operands (type Var) have been declared.
		if ((ctx->children.front()->ruleType == CMINUS_RULE_TYPE::RuleVar) && (!ctx->children.front()->symbolTableRecord->canBeUsed())) {
			// Print if left operand is undeclared.
			this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::UNDECL_IDENTIFIER,
				ctx->children.front()->symbolTableRecord->token->getLine(), ("Left operand "
					+ ctx->children.front()->symbolTableRecord->token->getText() + " of "
					+ ParseTreeRuleNames[static_cast<std::size_t>(ctx->ruleType)] + " is undeclared."));
		}
		// Indicate that the left operand is being used.
		ctx->children.front()->symbolTableRecord->isUsed = true;
	}
	if (ctx->children.back()->symbolTableRecord != NULL) {
		if ((ctx->children.back()->ruleType == CMINUS_RULE_TYPE::RuleVar) && (!ctx->children.back()->symbolTableRecord->canBeUsed())) {
			// Print if right operand is undeclared.
			this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::UNDECL_IDENTIFIER,
				ctx->children.back()->symbolTableRecord->token->getLine(), ("Right operand "
					+ ctx->children.back()->symbolTableRecord->token->getText() + " of "
					+ ParseTreeRuleNames[static_cast<std::size_t>(ctx->ruleType)] + " is undeclared."));
		}
		// Indicate that the right operand is being used.
		ctx->children.back()->symbolTableRecord->isUsed = true;
	}
	// TODO: Add warning printouts here to check for variables that are unassigned.
}

/** Define a helper function to get the function declaration which defines the current scope. */
SymbolRecord::SymbolRecordPtrType const & AstVisitorImpl::getCurrentScopeFunctionDeclaration(AstNode * ctx) {
	AstNode * curNode = ctx;
	do {
		// Go up one level in the tree.
		curNode = curNode->parent;
	} while (curNode->ruleType != CMINUS_RULE_TYPE::RuleFunDeclaration);
	return curNode->symbolTableRecord;
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
	// TODO: For array parameters, their storage size may vary depending on the call. This needs to updated in activation records.
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitCompoundStmt(AstNode * ctx) {
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitLocalDeclaration(AstNode * ctx) {
	this->visitChildren(ctx);
}
void AstVisitorImpl::visitStatementList(AstNode * ctx) {
	// If any children are empty ExpressionStatements, remove them.
	ctx->children.erase(std::remove_if(ctx->children.begin(), ctx->children.end(),
			[](AstNode * const & curStatementNode) { 
				return (curStatementNode->ruleType == CMINUS_RULE_TYPE::RuleExpressionStmt); 
			}
		), ctx->children.end());
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
		// Check if return value is a single variable, and if it has been declared yet.
		if (ctx->children.front()->symbolTableRecord != nullptr) {
			if (ctx->children.front()->symbolTableRecord->canBeUsed()) {
				// Indicate that this variable is being used.
				ctx->children.front()->symbolTableRecord->isUsed = true;
			} else {
				// Illegal to use before declared! Print error.
				this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::UNDECL_IDENTIFIER, 
						ctx->children.front()->symbolTableRecord->token->getLine(), ("Undeclared variable "
								+ ctx->children.front()->symbolTableRecord->text + " returned from function!"));
			}
		}
		// If we return a value, it must be an INT type.
		if (this->getCurrentScopeFunctionDeclaration(ctx)->returnType != CMINUS_NATIVE_TYPES::INT) {
			// Print an error indicating we are returning a value, but return type in function declaration is VOID.
			this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::NO_RETURN_VAL,
					ctx->children.front()->symbolTableRecord->token->getLine(), ("Declaration of function " 
							+ this->getCurrentScopeFunctionDeclaration(ctx)->text + " specified VOID return type, but the function returns a value."));
		} 
		if (ctx->children.front()->evaluatedType != CMINUS_NATIVE_TYPES::INT) {
			// Print an error indicating invalid return type.
			this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::INVALID_TYPE,
					ctx->children.front()->symbolTableRecord->token->getLine(), ("Return value of function "
							+ this->getCurrentScopeFunctionDeclaration(ctx)->text + " is type VOID or UNKNOWN, expected INT."));
		}
	} else {
		// Check for the case in which we expected a return value, but did not have one.
		if (this->getCurrentScopeFunctionDeclaration(ctx)->returnType == CMINUS_NATIVE_TYPES::INT) {
			// Print an error indicating a missing return type.
			this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::MISSING_RETURN_VAL,
					ctx->token->getLine(), ("Return value of type INT expected for function "
							+ this->getCurrentScopeFunctionDeclaration(ctx)->text + "."));
		}
	}
}

void AstVisitorImpl::visitExpression(AstNode * ctx) {
	this->visitChildren(ctx);
	// If this is an assignment ('='), ensure an r-value is being assigned to an l-value.
	if (ctx->children.size() == 2) {
		if (!ctx->children.front()->isLValue || !ctx->children.back()->isRValue) {
			this->compiler->getErrorHandler()->printError(ErrorHandler::ErrorCodes::LVAL_MISUSE,
					ctx->children.back()->symbolTableRecord->token->getLine(), ("The L-value "
							+ ctx->children.back()->symbolTableRecord->text + " cannot be assigned to a variable."));
		}
	}
}

void AstVisitorImpl::visitVar(AstNode * ctx) {
	this->visitChildren(ctx);
	// If this variable has been assigned a value, then it is also an r-value.
	if (ctx->symbolTableRecord->isAssigned) {
		ctx->isRValue = true;
	}
}

// TODO: Evaluate the values at each of these nodes, if possible. Can we do this with separate SemanticAnalysis call and walk?
void AstVisitorImpl::visitSimpleExpression(AstNode * ctx) {
	this->visitChildren(ctx);
	// Ensure the operands have been declared.
	this->verifyOperandUsability(ctx);
	// Ensure the operands for the relative operation are INT types.
	this->verifyMathOperandTypes(ctx);
}

void AstVisitorImpl::visitAdditiveExpression(AstNode * ctx) {
	this->visitChildren(ctx);
	// Ensure the operands have been declared.
	this->verifyOperandUsability(ctx);
	// Ensure the operands for the addition/subtraction operation are INT types.
	this->verifyMathOperandTypes(ctx);
}

void AstVisitorImpl::visitTerm(AstNode * ctx) {
	this->visitChildren(ctx);
	// Ensure the operands have been declared.
	this->verifyOperandUsability(ctx);
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




