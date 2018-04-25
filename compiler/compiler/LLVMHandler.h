#ifndef LLVMHANDLER_H
#define LLVMHANDLER_H

/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: LLVMHandler
 */

#include <list>
#include <string>

#include <llvm/IR/IRBuilder.h>

namespace llvm {
	class LLVMContext;
	class Module;
}

struct LLVMHandler {

	/** Alias some commonly used types for convenience. */
	typedef std::string LLVMInstructionType;
	
	/** Define the constructors. */
	LLVMHandler();
	~LLVMHandler();
	
	/** Define a function to print the generated LLVM strings. */
	void printAll();
	/** Define some LLVM-specific constructs to support IR generation. */
	llvm::LLVMContext * context;
	llvm::IRBuilder<> * builder;
	std::unique_ptr<llvm::Module> llvmModule;
	//static std::map<std::string, Value *> NamedValues;

	/** Define the list of LLVM commands to be generated. */
	std::list<LLVMInstructionType> llvmList;
};


#endif /* LLVMHANDLER_H */
