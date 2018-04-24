#ifndef LLVMHANDLER_H
#define LLVMHANDLER_H

/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: LLVMHandler
 */

#include <list>
#include <string>

struct LLVMHandler {

	/** Alias some commonly used types for convenience. */
	typedef std::string LLVMInstructionType;
	
	/** Define the constructors. */
	LLVMHandler();
	~LLVMHandler();
	
	/** Define the list of LLVM commands to be generated. */
	std::list<LLVMInstructionType> llvmList;
};


#endif /* LLVMHANDLER_H */
