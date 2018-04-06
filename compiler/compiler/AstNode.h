#ifndef ASTNODE_H
#define ASTNODE_H

/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: AstNode
 */

#include "ParseTree.h"
#include "CPPUtils.h"
#include "LanguageDefinitions.h"


/** Forward declarations. */
struct SymbolRecord;

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


#endif  /* ASTNODE_H */
