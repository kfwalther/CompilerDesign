/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: SymbolRecord/SymbolTable
 */

#include "SymbolTable.h"


SymbolRecord::SymbolRecord() {
	return;
}

SymbolRecord::SymbolRecord(antlr4::Token * const & inputToken) {
	this->token = inputToken;
	this->text = inputToken->getText();
	return;
}

/** Define a method to quickly determine if the entry can be used (when it's declared/defined). */
bool SymbolRecord::canBeUsed() {
	// We can still use an unassigned variable, so only check if declared/defined.
	if (this->isDeclared && this->isDefined) {
		return true;
	} else {
		return false;
	}
}

SymbolTable::SymbolTable() {
	return;
}

/** Insert the new SymbolTableRecord into the symbol table, if it does not already exist. */
bool SymbolTable::insertSymbol(SymbolRecord::SymbolRecordPtrType const & newSymbol) {
	/** Check if the symbol already exists in the table. */
	if (this->symbolTable.count(newSymbol->text) == 0) {
		this->symbolTable[newSymbol->text] = newSymbol;
		return true;
	}
	else {
		//std::cout << "WARNING: Value " << newSymbol->text << " already in map..." << std::endl;
		return false;
	}
}

/** Print out the contents of the symbol table. */
void SymbolTable::printSymbolTable() const {
	std::cout << "SYMBOL TABLE PRINTOUT" << std::endl;
	for (auto const & entry : this->symbolTable) {
		std::cout << entry.first << ", " << entry.second->token->toString() << std::endl;
		std::cout << "Type: " << static_cast<unsigned int>(entry.second->type) 
				<< ", Kind: " << static_cast<unsigned int>(entry.second->kind) << std::endl;
		std::cout << "StorageSize: " << entry.second->storageSize << std::endl;
	}

}