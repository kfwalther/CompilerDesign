/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: LLVMHandler
 */

#include <string>
#include <iostream>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include "LLVMHandler.h"

LLVMHandler::LLVMHandler() {
	// Initialize the LLVM-specific constructs.
	this->context = new llvm::LLVMContext();
	this->builder = new llvm::IRBuilder<>(*this->context);
	this->llvmModule = std::make_unique<llvm::Module>("C-Minus LLVM", *this->context);
	return;
}

LLVMHandler::~LLVMHandler() {
	return;
}

/** Define a helper function to create an alloca instruction in the entry block of
	the function.  This is used for mutable variables etc. */
llvm::AllocaInst * LLVMHandler::createEntryBlockAlloca(
		llvm::Function * llvmFunction, std::string const & variableName) {
	llvm::IRBuilder<> tempBlock(&llvmFunction->getEntryBlock(), llvmFunction->getEntryBlock().begin());
	return tempBlock.CreateAlloca(llvm::Type::getInt32Ty(*this->context), 0, variableName.c_str());
}

/** Define a function to save the LLVM Value as a string in the list. */
void LLVMHandler::saveLLVMInstruction(llvm::Value * llvmValue) {
	//llvmValue->print(llvm::errs());
	std::string temp;
	llvm::raw_string_ostream rsos(temp);
	rsos << llvmValue;
	rsos.flush();
	this->llvmList.push_back(temp);
}

/** Define a function to print the generated LLVM strings. */
void LLVMHandler::printAll() {
	std::cout << std::endl << "LLVM DUMP" << std::endl;
	this->llvmModule->print(llvm::errs(),nullptr);
	//for (auto const & curEntry : this->llvmList) {
	//	std::cout << curEntry << std::endl;
	//}
}

