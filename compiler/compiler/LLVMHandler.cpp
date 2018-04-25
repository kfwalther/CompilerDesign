/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: LLVMHandler
 */

#include <string>
#include <iostream>

#include "LLVMHandler.h"

LLVMHandler::LLVMHandler() {
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

