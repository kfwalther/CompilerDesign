/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: SemanticAnalyzer
 */

#include "SemanticAnalyzer.h"

SemanticAnalyzer::SemanticAnalyzer() {
	return;
}

SemanticAnalyzer::~SemanticAnalyzer() {
	return;
}

/** Define a function to store a SymbolTable record in memory. */
void SemanticAnalyzer::storeRecordInMemory(SymbolRecord::SymbolRecordPtrType const & symbolRecord) {
	// Get the current available space in the heap memory for this variable.
	symbolRecord->memoryLocation = this->curHeapMemoryAddress;
	// Update the current available space in the heap, based on the storage size of the variable.
	this->curHeapMemoryAddress += symbolRecord->storageSize;
}

