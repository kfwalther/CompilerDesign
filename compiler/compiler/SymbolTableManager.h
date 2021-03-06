#ifndef SYMBOLTABLEMANAGER_H
#define SYMBOLTABLEMANAGER_H

/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: SymbolTableManager
 */

#include <stack>
#include "SymbolTable.h"

/** Forward declarations. */
struct AstNode;
struct Scope;


/** Define an object to track all of the symbol tables in different scopes. */
struct SymbolTableManager {
	/** Alias the types used with this class. */
	typedef SymbolTable::SymbolTablePtrType SymbolTablePtrType;
	typedef std::shared_ptr<Scope> ScopePtrType;

	/** Define constructors. */
	SymbolTableManager();

	/** Initialize the stack of scopes containing the symbol tables. */
	void initialize();
	/** Retrieve the next scope unique ID. */
	unsigned int const getNextUniqueId();
	/** Create a new local scope and add it to the stack. */
	ScopePtrType newScope();
	/** Get rid of the current scope. */
	void popScope();
	/** Get the current scope. */
	ScopePtrType getCurrentScope();
	ScopePtrType getCurrentScopeFromVector();


	/** Define the stack of scopes that we will populate to keep track of symbol tables. */
	std::stack<ScopePtrType> scopeStack;
	std::vector<ScopePtrType> scopeVector;
	/** Define the unique ID counter for the managed scopes. */
	unsigned int scopeIdCounter = 0;
	/** Define an ID type to keep track of the current scope as we walk the AST. */
	size_t currentScopeId = 0;
};


#endif	/* SYMBOLTABLEMANAGER_H */