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

/** Define a method to quickly determine if the entry can be used (when it's declared/defined and assigned). */
bool SymbolRecord::canBeUsed() {
	if (this->isDeclared && this->isDefined && this->isAssigned) {
		return true;
	} else {
		return false;
	}
}

SymbolTable::SymbolTable() {
	/** Add the input() and output() function names to the symbol table. They have the following signature:
		int input(void) { ... }
		void output(int x) { ... }
	*/
	// Add the input() function.
	std::shared_ptr<SymbolRecord> inputFunctionRecord = std::make_shared<SymbolRecord>();
	//inputFunctionRecord->type = CMINUS_NATIVE_TYPES::INT;
	//inputFunctionRecord->returnType = CMINUS_NATIVE_TYPES::INT;
	inputFunctionRecord->kind = SYMBOL_RECORD_KIND::FUNCTION;
	inputFunctionRecord->isDeclared = true;
	inputFunctionRecord->isDefined = true;
	inputFunctionRecord->numArguments = 0;
	this->symbolTable["input"] = inputFunctionRecord;
	// Add the output() function.
	std::shared_ptr<SymbolRecord> outputFunctionRecord = std::make_shared<SymbolRecord>();
	//outputFunctionRecord->type = CMINUS_NATIVE_TYPES::VOID;
	//outputFunctionRecord->returnType = CMINUS_NATIVE_TYPES::VOID;
	outputFunctionRecord->kind = SYMBOL_RECORD_KIND::FUNCTION;
	outputFunctionRecord->isDeclared = true;
	outputFunctionRecord->isDefined = true;
	outputFunctionRecord->numArguments = 1;
	this->symbolTable["output"] = outputFunctionRecord;
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
		//std::cout << "WARNING: Value " << newSymbol->token->getText() << " already in map..." << std::endl;
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