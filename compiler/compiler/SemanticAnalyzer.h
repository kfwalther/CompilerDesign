#ifndef SEMANTICANALYZER_H
#define SEMANTICANALYZER_H

/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: SemanticAnalyzer
 */

#include <cstdlib>
#include "SymbolTable.h"

struct SemanticAnalyzer {

	/** Define the constructors. */
	SemanticAnalyzer();
	~SemanticAnalyzer();
	
	/** Define a function to store a SymbolTable record in memory. */
	void storeRecordInMemory(SymbolRecord::SymbolRecordPtrType const & symbolRecord);
	/** TODO: Write functions to check for Constant Propagation, Constant Folding, and Dead Code Removal opportunities. */
	/** Define a pointer to the first open memory space in heap memory. */
	std::size_t curHeapMemoryAddress = 0;
	/** Define pointers to the location of the stack in memory. */

};


#endif /* SEMANTICANALYZER_H */
