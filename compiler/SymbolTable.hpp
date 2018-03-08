/**
 * @author: Kevin Walther
 */

#include <string>
#include <iostream>
//#include <memory>
#include <map>

/** Define an object to store a single entry in the symbol table. */
struct SymbolRecord {
	/** Alias the types used with this class. */
	typedef std::string SymbolNameType;
	typedef int SymbolValueType;
	typedef SymbolRecord * SymbolRecordPtrType;
	
	/** Define constructors. */
	SymbolRecord();
	SymbolRecord(SymbolNameType newSymbolName, SymbolValueType newSymbolValue);

	/** Define the attributes to keep track of for each symbol. */
	SymbolNameType symbolName;
	SymbolValueType symbolValue;

};

/** Define an object to store all of the symbols in the symbol table. */
struct SymbolTable {
	/** Alias the types used with this class. */
	typedef std::map<SymbolRecord::SymbolNameType, SymbolRecord> SymbolTableType;
	typedef SymbolTable * SymbolTablePtrType;

	/** Define constructors. */
	SymbolTable();

	/** Define the map data structure to hold all of the symbol table entries. */
	SymbolTableType symbolTable;

};



