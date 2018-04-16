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
	return;
};

/** Define a symbol in the current scope. */
void Scope::newSymbol(SymbolRecord::SymbolRecordPtrType const & newSymbolRecord) {
	newSymbolRecord->setScope(shared_from_this());
	this->scopedSymbolTable->symbolTable[newSymbolRecord->text] = newSymbolRecord;
}

/** Define a function to lookup up a symbol in the current scope, then in the enclosing scopes. */
Scope::OptionalSymbolRecordPtrType const & Scope::findSymbol(std::string const & symbolName) {
	auto matchingSymbolIterator = this->scopedSymbolTable->symbolTable.find(symbolName);
	if (matchingSymbolIterator != this->scopedSymbolTable->symbolTable.end()) {
		return std::optional<SymbolRecordPtrType>{matchingSymbolIterator->second};
	} else if (this->enclosingScope != NULL) {
		return this->enclosingScope->findSymbol(symbolName);
	}
	return std::nullopt; 
}


//std::string const toString() {
//	return this->scopedSymbolTable.toString();
//}

