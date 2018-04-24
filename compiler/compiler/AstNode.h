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
struct SymbolTable;
namespace llvm {
	class Value;
}

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
	/** Generate the LLVM code for this node. */
	llvm::Value * generateLLVM();

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
	bool isLValue = false;
	bool isRValue = false;
	
	CMINUS_RULE_TYPE ruleType;
	CMINUS_TOKEN_TYPE tokenType;
	/** Define the effective type this node will be evaluated as. */
	CMINUS_NATIVE_TYPES evaluatedType;
	/** Define the effective value this node will be evaluated as. */
	int evaluatedValue;
	/** Define the LLVM value this node will have. */
	//llvm::Value * llvmValue;
};


#endif  /* ASTNODE_H */
