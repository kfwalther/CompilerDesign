#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

/**
 * @author: Kevin Walther
 */

#include <string>
#include <iostream>
#include <memory>
#include <map>
#include "Token.h"

/** Forward declarations. */
struct AstNode;


enum class SYMBOL_RECORD_TYPE {
	INT = 0,
	ARRAY = 1,
	FUNCTION = 2
};

/** Define an object to store a single entry in the symbol table. */
struct SymbolRecord {
	/** Alias the types used with this class. */
	typedef std::shared_ptr<SymbolRecord> SymbolRecordPtrType;
	typedef antlr4::Token * TokenPtrType;
	typedef std::shared_ptr<AstNode> AstNodePtrType;
	/** Define constructors. */
	SymbolRecord();
	SymbolRecord(antlr4::Token * const & inputToken);

	/** Define the type of this symbol table record. */
	SYMBOL_RECORD_TYPE type;
	/** Define how much memory this entity will require. */
	size_t storageSize;

	/** Define the return type if this is a function. */
	std::string returnType;
	/** Define number of arguments if this is a function. */
	size_t numArguments;

	TokenPtrType token;

	/** Pointer to corresponding AST node. */
	AstNodePtrType astNode;
	/** Define attributes to track how each token is used in the program. */
	bool isDeclared = false;
	bool isDefined = false;
	bool isAssigned = false;
	bool isUsed = false;

};

/** Define an object to store all of the symbols in the symbol table. */
struct SymbolTable {
	/** Alias the types used with this class. */
	typedef std::map<std::string const, SymbolRecord::SymbolRecordPtrType> SymbolTableType;
	typedef std::shared_ptr<SymbolTable> SymbolTablePtrType;

	/** Define constructors. */
	SymbolTable();

	/** Define a function to insert a new token into the symbol table, if it doesn't already exist. */
	bool insertSymbol(SymbolRecord::SymbolRecordPtrType const & newSymbol);
	void printSymbolTable();
	/** Define the map data structure to hold all of the symbol table entries. */
	SymbolTableType symbolTable;

};


#endif SYMBOLTABLE_H	