#ifndef ASTNODE_H
#define ASTNODE_H

/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "TerminalNode.h"

/** Copy these from the TParser.h generated file for use in our custom data structures. */
enum class CMINUS_RULE_TYPE {
	RuleProgram = 0, RuleDeclarationList = 1, RuleDeclaration = 2, RuleVarDeclaration = 3,
	RuleTypeSpecifier = 4, RuleFunDeclaration = 5, RuleParams = 6, RuleParamList = 7,
	RuleParam = 8, RuleCompoundStmt = 9, RuleLocalDeclaration = 10, RuleStatementList = 11,
	RuleStatement = 12, RuleExpressionStmt = 13, RuleSelectionStmt = 14,
	RuleIterationStmt = 15, RuleReturnStmt = 16, RuleExpression = 17, RuleVar = 18,
	RuleSimpleExpression = 19, RuleRelop = 20, RuleAdditiveExpression = 21,
	RuleAddop = 22, RuleTerm = 23, RuleMulop = 24, RuleFactor = 25, RuleCall = 26,
	RuleArgs = 27, RuleArgList = 28
};

enum class CMINUS_TOKEN_TYPE {
	DUMMY = 1, INT_KEYWORD = 2, VOID_KEYWORD = 3, IF_KEYWORD = 4, ELSE_KEYWORD = 5,
	WHILE_KEYWORD = 6, RETURN_KEYWORD = 7, NUM = 8, DIGIT = 9, ID = 10,
	LETTER = 11, COMMENT = 12, WS = 13, ASSIGN = 14, PLUS = 15, MINUS = 16,
	MULTIPLY = 17, DIVIDE = 18, LESS_THAN = 19, GREATER_THAN = 20, LEFT_PAREN = 21,
	RIGHT_PAREN = 22, LEFT_BRACES = 23, RIGHT_BRACES = 24, LEFT_BRACKET = 25,
	RIGHT_BRACKET = 26, COMMA = 27, SEMICOLON = 28, LESS_THAN_OR_EQUAL = 29,
	GREATER_THAN_OR_EQUAL = 30, EQUALITY = 31, NON_EQUALITY = 32
};

// TODO: Create a wrapper around this type, or make Terminal node wrapper to contruct AST.
struct AstNode {
	/** Define the constructors. */
	AstNode(CMINUS_RULE_TYPE const ruleType);
	AstNode(antlr4::tree::TerminalNode * inputNode);
	~AstNode();
	AstNode * parent;
	std::vector<AstNode *> children;
	
	CMINUS_RULE_TYPE ruleType;
	CMINUS_TOKEN_TYPE tokenType;
};


#endif ASTNODE_H