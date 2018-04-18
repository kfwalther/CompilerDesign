#ifndef SCOPE_H
#define SCOPE_H

/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: Scope
 */

#include <optional>
#include <memory>
#include "SymbolTable.h"
#include "LanguageDefinitions.h"


/** Define an object to keep track of a single scope. */
/** Implementation inspired by the following scoped symbol table post:
	https://stackoverflow.com/questions/15588452/build-symbol-table-from-grammar */
struct Scope : public std::enable_shared_from_this<Scope> {
	/** Alias the types used with this class. */
	typedef std::shared_ptr<Scope> ScopePtrType;
	typedef SymbolRecord::SymbolRecordPtrType SymbolRecordPtrType;
	typedef SymbolTable::SymbolTablePtrType SymbolTablePtrType;

	/** Define constructors. */
	Scope(CMINUS_SCOPE_TYPE const & scopeType, unsigned int const id, ScopePtrType const & enclosingScope);

	/** Define a function to add a symbol to this scope. */
	void newSymbol(SymbolRecord::SymbolRecordPtrType const & newSymbolRecord);
	/** Define a function to lookup up a symbol in the current scope, then in the enclosing scopes. */
	SymbolRecordPtrType const & findSymbol(std::string const & name);
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


