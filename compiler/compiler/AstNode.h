#ifndef ASTNODE_H
#define ASTNODE_H

/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "ParseTree.h"
#include "CPPUtils.h"

/** Forward declarations. */
struct SymbolRecord;

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

/** Define the C-Minus language native types. */
enum class CMINUS_NATIVE_TYPES {
	UNKNOWN = 0,
	INT = 1,
	VOID = 2
};

/** Define the static list of rule names for the parse tree. */
static const std::vector<std::string> ParseTreeRuleNames = {
	"program", "declarationList", "declaration", "varDeclaration", "typeSpecifier",
	"funDeclaration", "params", "paramList", "param", "compoundStmt", "localDeclaration",
	"statementList", "statement", "expressionStmt", "selectionStmt", "iterationStmt",
	"returnStmt", "expression", "var", "simpleExpression", "relop", "additiveExpression",
	"addop", "term", "mulop", "factor", "call", "args", "argList"
};

// TODO: Create a wrapper around this type, or make Terminal node wrapper to contruct AST.
struct AstNode {

	/** Alias some commonly used types for convenience. */
	typedef AstNode * AstNodePtrType;
	typedef std::shared_ptr<AstNode> AstNodeSmartPtrType;
	typedef antlr4::Token * TokenPtrType;
	typedef std::shared_ptr<SymbolRecord> SymbolRecordPtrType;
	typedef std::vector<AstNodePtrType> AstNodePtrVectorType;

	/** Define the constructors. */
	AstNode(CMINUS_RULE_TYPE const ruleType);
	AstNode(antlr4::tree::ParseTree * inputNode);
	AstNode(antlr4::tree::ParseTree * inputNode, AstNodePtrType const & parentNode);
	/** Define a copy constructor. */
	AstNode(AstNode * const & otherAstNode);
	~AstNode();

	/** Initialize some of the data members. */
	void initialize(antlr4::tree::ParseTree * inputNode);

	bool hasToken() const;
	std::string const getString() const;
	std::string printTreeString();

	AstNodePtrType parent;
	AstNodePtrVectorType children;
	/** Store a pointer to the token, if this node has a corresponding Token. */
	TokenPtrType token;
	/** Store a pointer to the symbol table record this corresponds to. */
	SymbolRecordPtrType symbolTableRecord;
	/** Define booleans to indicate when this node is a variable, and is an L-value or R-value. */
	// TODO: Incorporate use of these in AST-creation visitors.
	bool isLValue = false;
	bool isRValue = false;
	
	CMINUS_RULE_TYPE ruleType;
	CMINUS_TOKEN_TYPE tokenType;
};


#endif ASTNODE_H
