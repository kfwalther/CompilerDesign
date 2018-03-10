/**
 * @author: Kevin Walther
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

void SymbolTable::printSymbolTable() {
	std::cout << "SYMBOL TABLE PRINTOUT" << std::endl;
	for (auto const & entry : this->symbolTable) {
		std::cout << entry.first << ", " << entry.second->token->toString() << std::endl;
	}

}