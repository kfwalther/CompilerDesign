#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: SymbolTable
 */

#include <string>
#include <iostream>
#include <memory>
#include <map>
#include "Token.h"
#include "LanguageDefinitions.h"

/** Forward declarations. */
struct AstNode;
struct Scope;
namespace llvm {
	class AllocaInst;
}

/** Define an object to store a single entry in the symbol table. */
struct SymbolRecord {
	/** Alias the types used with this class. */
	typedef std::shared_ptr<SymbolRecord> SymbolRecordPtrType;
	typedef antlr4::Token * TokenPtrType;
	typedef std::shared_ptr<AstNode> AstNodeSmartPtrType;
	typedef std::shared_ptr<Scope> ScopePtrType;

	/** Define constructors. */
	SymbolRecord();
	SymbolRecord(antlr4::Token * const & inputToken);
	
	/** Set the scope for this symbol record. */
	void setScope(ScopePtrType const & scope);
	/** Define the kind of this symbol table record. */
	SYMBOL_RECORD_KIND kind = SYMBOL_RECORD_KIND::UNKNOWN;
	/** Define the type of this symbol table record (INT or VOID). */
	CMINUS_NATIVE_TYPES type = CMINUS_NATIVE_TYPES::UNKNOWN;
	/** Define how much memory in bytes this entity will require. Assume 32-bit integers. */
	std::size_t storageSize = 0;
	/** Define the location in memory where this is stored. */
	std::size_t memoryLocation;
	/** Duplicate the text name of this symbol for convenience. */
	std::string text;
	/** If this is a variable, define the value it takes on (assume zero-initialized memory). */
	int value;
	// Use this to look up LLVM values in symbol table (see Kaleidoscope example)!
	llvm::AllocaInst * llvmValue;

	/** Define the return type if this is a function. */
	//TODO: Do we need return type, it seems redundant with type above...
	CMINUS_NATIVE_TYPES returnType = CMINUS_NATIVE_TYPES::UNKNOWN;
	/** Define number of arguments if this is a function. */
	std::size_t numArguments;
	/** TODO: Consider vector for function arguments (type, name, value, storage location, or link to other record). 
		Removes the need for numArguments attribute above. */
	// std::vector< std::tuple< CMINUS_NATIVE_TYPES, std::string, int, unsignedInt > >


	/** Pointer to one of its corresponding AST nodes. */
	AstNodeSmartPtrType astNode;
	TokenPtrType token;
	/** Pointer to the scope this symbol record belongs to. */
	ScopePtrType scope;

	/** Define attributes to track how each token is used in the program. */
	bool isDeclared = false;
	bool isDefined = false;
	bool isAssigned = false;
	bool isUsed = false;
	/** Define a method to quickly determine if the entry can be used (when it's declared/defined and assigned). */
	bool canBeUsed();
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
	/** Define functions to create a new SymbolRecord, initialize it, and insert it into the symbol table. */
	void emplaceSymbol(antlr4::Token * const & inputToken);
	void emplaceSymbol(antlr4::Token * const & inputToken, AstNode * const & correspondingAstNode);
	void printSymbolTable() const;
	/** Define the map data structure to hold all of the symbol table entries. */
	SymbolTableType symbolTable;
};


#endif	/* SYMBOLTABLE_H */