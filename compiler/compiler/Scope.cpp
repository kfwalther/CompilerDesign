/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: Scope
 */

#include "Scope.h"

/** Define a default constructor. */
Scope::Scope(CMINUS_SCOPE_TYPE const & scopeType, unsigned int const id, ScopePtrType const & enclosingScope) :
		type(scopeType),
		uniqueId(id),
		enclosingScope(enclosingScope)
{
	this->scopedSymbolTable = std::make_shared<SymbolTable>();
	return;
};

/** Define a symbol in the current scope. */
void Scope::newSymbol(Scope::SymbolRecordPtrType const & newSymbolRecord) {
	newSymbolRecord->setScope(shared_from_this());
	this->scopedSymbolTable->symbolTable[newSymbolRecord->text] = newSymbolRecord;
}

/** Define a function to lookup up a symbol in the current scope, then in the enclosing scopes. */
Scope::SymbolRecordPtrType const & Scope::findSymbol(std::string const & symbolName) {
	auto matchingSymbolIterator = this->scopedSymbolTable->symbolTable.find(symbolName);
	if (matchingSymbolIterator != this->scopedSymbolTable->symbolTable.end()) {
		return matchingSymbolIterator->second;
	} else if (this->enclosingScope != NULL) {
		return this->enclosingScope->findSymbol(symbolName);
	}
	return nullptr; 
}

/** Define a function to print the scope information. */
void Scope::print() {
	std::cout << std::endl << ScopeTypeNames[static_cast<unsigned int>(this->type)] << " SCOPE: ID "
			<< this->uniqueId;
}


