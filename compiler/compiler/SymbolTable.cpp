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
	return;
}


SymbolTable::SymbolTable() {
	return;
}

/** Insert the new SymbolTableRecord into the symbol table, if it does not already exist. */
bool SymbolTable::insertSymbol(SymbolRecord::SymbolRecordPtrType const & newSymbol) {
	/** Check if the symbol already exists in the table. */
	if (this->symbolTable.count(newSymbol->token->getText()) == 0) {
		this->symbolTable[newSymbol->token->getText()] = newSymbol;
		return true;
	}
	else {
		std::cout << "WARNING: Value " << newSymbol->token->getText() << " already in map..." << std::endl;
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