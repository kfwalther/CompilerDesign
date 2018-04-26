/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: SymbolTableManager
 */

#include "SymbolTableManager.h"
#include "ErrorHandler.h"
#include "Scope.h"

SymbolTableManager::SymbolTableManager() {
	// Initialize the symbol table manager.
	this->initialize();
	return;
}

/** Initialize the stack of scopes containing the symbol tables. */
void SymbolTableManager::initialize() {
	// Create the global scope.
	ScopePtrType globalScope = std::make_shared<Scope>(CMINUS_SCOPE_TYPE::GLOBAL, this->getNextUniqueId(), nullptr);
	this->scopeStack.push(globalScope);
	this->scopeVector.push_back(globalScope);
}

/** Retrieve the next scope unique ID. */
unsigned int const SymbolTableManager::getNextUniqueId() {
	return this->scopeIdCounter++;
}

/** Create a new local scope and add it to the stack. */
SymbolTableManager::ScopePtrType SymbolTableManager::newScope() {
	SymbolTableManager::ScopePtrType enclosingScope = this->scopeStack.top();
	// Create a new local scope with a unique ID.
	SymbolTableManager::ScopePtrType scope = std::make_shared<Scope>(
			CMINUS_SCOPE_TYPE::LOCAL, this->getNextUniqueId(), enclosingScope);
	// Add it to both scope data structures.
	this->scopeStack.push(scope);
	this->scopeVector.push_back(scope);
	return scope;
}

/** Throw out the current scope. */
void SymbolTableManager::popScope() {
	this->scopeStack.pop();
}

/** Get the current scope. */
SymbolTableManager::ScopePtrType SymbolTableManager::getCurrentScope() {
	if (this->scopeStack.size() > 0) {
		return this->scopeStack.top();
	}
	std::cerr << ErrorHandler::ErrorCodes::COMPILER_ERROR << "Unbalanced scope stack." << std::endl << std::endl;
	return nullptr;
}
SymbolTableManager::ScopePtrType SymbolTableManager::getCurrentScopeFromVector() {
	if (this->currentScopeId < this->scopeVector.size()) {
		return this->scopeVector.at(this->currentScopeId);
	}
	std::cerr << ErrorHandler::ErrorCodes::COMPILER_ERROR << "Invalid index used for scope vector." << std::endl << std::endl;
	return nullptr;
}

//Scope getScope(int genId) {
//	for (Scope scope : scopeStack) {
//		if (scope.genId == genId) return scope;
//	}
//	return null;
//}



