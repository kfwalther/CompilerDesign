#ifndef SCOPE_H
#define SCOPE_H

/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: Scope
 */

#include <memory>
//#include "SymbolTable.h"
#include "LanguageDefinitions.h"

/** Forward declarations. */
struct SymbolTable;
struct SymbolRecord;

/** Define an object to keep track of a single scope. */
/** Implementation inspired by the following scoped symbol table post:
	https://stackoverflow.com/questions/15588452/build-symbol-table-from-grammar */
struct Scope {
	/** Alias the types used with this class. */
	typedef std::shared_ptr<Scope> ScopePtrType;
	typedef SymbolTable::SymbolTablePtrType SymbolTablePtrType;

	/** Define constructors. */
	Scope(CMINUS_SCOPE_TYPE const & scopeType, unsigned int const id, Scope * const & enclosingScope);

	/** Define a function to add a symbol to this scope. */
	void newSymbol(SymbolRecord * const & newSymbolRecord);

	/** Define a unique ID to distinguish this scope. */
	unsigned int uniqueId;

	/** Define the type of this scope. */
	CMINUS_SCOPE_TYPE type;
	/** Define the scope one-level up from this scope (the parent scope). */
	ScopePtrType enclosingScope;
	/** Define the symbol table for this scope. */
	SymbolTablePtrType scopedSymbolTable;

};


#endif	/* SCOPE_H */


