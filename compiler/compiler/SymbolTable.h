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
enum class CMINUS_NATIVE_TYPES;

enum class SYMBOL_RECORD_KIND {
	UNKNOWN = 0,
	VARIABLE = 1,
	ARRAY = 2,
	FUNCTION = 3,
	NUMBER = 4
};

/** Define an object to store a single entry in the symbol table. */
struct SymbolRecord {
	/** Alias the types used with this class. */
	typedef std::shared_ptr<SymbolRecord> SymbolRecordPtrType;
	typedef antlr4::Token * TokenPtrType;
	typedef std::shared_ptr<AstNode> AstNodeSmartPtrType;

	/** Define constructors. */
	SymbolRecord();
	SymbolRecord(antlr4::Token * const & inputToken);
	
	/** Define the kind of this symbol table record. */
	SYMBOL_RECORD_KIND kind;
	/** Define the type of this symbol table record (INT or VOID). */
	CMINUS_NATIVE_TYPES type;
	/** Define how much memory in bytes this entity will require. Assume 32-bit integers. */
	size_t storageSize = 0;
	/** If this is a variable, define the value it takes on (assume zero-initialized memory). */
	int value;

	/** Define the return type if this is a function. */
	//TODO: Do we need return type, it seems redundant with type above...
	CMINUS_NATIVE_TYPES returnType;
	/** Define number of arguments if this is a function. */
	size_t numArguments;

	/** Pointer to one of its corresponding AST nodes. */
	AstNodeSmartPtrType astNode;
	TokenPtrType token;

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
	void printSymbolTable() const;
	/** Define the map data structure to hold all of the symbol table entries. */
	SymbolTableType symbolTable;

};


#endif SYMBOLTABLE_H	