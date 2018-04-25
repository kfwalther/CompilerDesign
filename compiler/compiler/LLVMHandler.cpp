/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: LLVMHandler
 */

#include <string>
#include <iostream>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "LLVMHandler.h"

LLVMHandler::LLVMHandler() {

	this->context = new llvm::LLVMContext();
	this->builder = new llvm::IRBuilder<>(*this->context);
	this->llvmModule = std::make_unique<llvm::Module>("C-Minus LLVM", *this->context);
	return;
}

LLVMHandler::~LLVMHandler() {
	return;
}

/** Define a function to print the generated LLVM strings. */
void LLVMHandler::printAll() {
	std::cout << std::endl << "LLVM DUMP" << std::endl;
	for (auto const & curEntry : this->llvmList) {
		std::cout << curEntry << std::endl;
	}
}

