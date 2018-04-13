/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: Scope
 */

#include "Scope.h"

/** Define a default constructor. */
Scope::Scope(CMINUS_SCOPE_TYPE const & scopeType, unsigned int const id, Scope * const & enclosingScope) :
		type(scopeType),
		uniqueId(id),
		enclosingScope(enclosingScope)
{
	return;
};

/** Define a symbol in the current scope. */
void Scope::newSymbol(SymbolRecord * const & newSymbolRecord) {
	newSymbolRecord->setScope(this);
	this->scopedSymbolTable[newSymbolRecord->text] = newSymbolRecord;
}

/**
* Look up the symbol name in this scope and, if not found,
* progressively search the enclosing scopes.
* Return null if not found in any applicable scope.
*/
//private Symbol resolve(String name) {
//	Symbol symbol = symbolMap.get(name);
//	if (symbol != null) return symbol;
//	if (enclosingScope != null) return enclosingScope.resolve(name);
//	return null; // not found
//}
//
//
//std::string const toString() {
//	return this->scopedSymbolTable.toString();
//}

