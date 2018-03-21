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

/** Define an object to store a single entry in the symbol table. */
struct SymbolRecord {
	/** Alias the types used with this class. */
	typedef std::shared_ptr<SymbolRecord> SymbolRecordPtrType;
	typedef antlr4::Token * TokenPtrType;
	typedef std::unique_ptr<antlr4::tree::TerminalNode> TreeTerminalNodePtrType;
	/** Define constructors. */
	SymbolRecord();
	SymbolRecord(antlr4::Token * const & inputToken);

	/** Define the attributes to keep track of for each symbol. */
	int additionalAttribute;
	TokenPtrType token;

	/** Pointer to corresponding AST node. */
	TreeTerminalNodePtrType astNodePtr;
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