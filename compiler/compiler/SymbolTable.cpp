/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: SymbolRecord/SymbolTable
 */

#include "SymbolTable.h"
#include "AstNode.h"

SymbolRecord::SymbolRecord() {
	return;
}

SymbolRecord::SymbolRecord(antlr4::Token * const & inputToken) {
	this->token = inputToken;
	this->text = inputToken->getText();
	this->llvmValue = nullptr;
	return;
}

/** Set the scope for this symbol record. */
void SymbolRecord::setScope(ScopePtrType const & scope) {
	this->scope = scope;
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
		std::cout << "WARNING: Value " << newSymbol->text << " already in map..." << std::endl;
		return false;
	}
}

/** Define a function to create a new SymbolRecord, initialize it, and insert it into the symbol table. */
void SymbolTable::emplaceSymbol(antlr4::Token * const & inputToken) {
	// NOTE: Currently only using this function to initialize NUM tokens, because they are consistent.
	// Save the information for the NUM token.
	std::shared_ptr<SymbolRecord> numSymbolRecord = std::make_shared<SymbolRecord>(inputToken);
	// Store some info about this integer number. 
	numSymbolRecord->type = CMINUS_NATIVE_TYPES::INT;
	numSymbolRecord->kind = SYMBOL_RECORD_KIND::NUMBER;
	// Initialize the NUM entry in the symbol table.
	numSymbolRecord->isDeclared = true;
	numSymbolRecord->isDefined = true;
	// Insert the new symbol record into the symbol table.
	this->insertSymbol(numSymbolRecord);
}


/** Define a function to create a new SymbolRecord, initialize it, and insert it into the symbol table. */
void SymbolTable::emplaceSymbol(antlr4::Token * const & inputToken, AstNode * const & correspondingAstNode) {
	// NOTE: Currently only using this function to initialize NUM tokens, because they are consistent.
	// Save the information for the NUM token.
	std::shared_ptr<SymbolRecord> numSymbolRecord = std::make_shared<SymbolRecord>(inputToken);
	// Store some info about this integer number. 
	numSymbolRecord->type = CMINUS_NATIVE_TYPES::INT;
	numSymbolRecord->kind = SYMBOL_RECORD_KIND::NUMBER;
	// Initialize the NUM entry in the symbol table.
	numSymbolRecord->isDeclared = true;
	numSymbolRecord->isDefined = true;
	numSymbolRecord->value = std::atoi(inputToken->getText().c_str());
	// Exchange pointers for association of an AST node with this symbol table entry.
	numSymbolRecord->astNode = std::make_shared<AstNode>(correspondingAstNode);
	correspondingAstNode->symbolTableRecord = numSymbolRecord;
	// Insert the new symbol record into the symbol table.
	this->insertSymbol(numSymbolRecord);
}

/** Print out the contents of the symbol table. */
void SymbolTable::printSymbolTable() const {
	std::cout << std::endl << "SYMBOL TABLE PRINTOUT" << std::endl << std::endl;
	for (auto const & entry : this->symbolTable) {
		std::cout << entry.first << ", " << entry.second->token->toString() << std::endl
				<< "Type: " << CMinusNativeTypeNames[static_cast<unsigned int>(entry.second->type)]
				<< ", Kind: " << SymbolRecordKindNames[static_cast<unsigned int>(entry.second->kind)] << std::endl
				<< "StorageSize: " << entry.second->storageSize << " bytes" << std::endl << std::endl;
	}
}