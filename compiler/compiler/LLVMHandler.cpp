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
#include <llvm/IR/Verifier.h>

#include "LLVMHandler.h"
#include "AstNode.h"
#include "SymbolTable.h"
#include "SymbolTableManager.h"
#include "Scope.h"
#include "Compiler.h"

LLVMHandler::LLVMHandler(Compiler * const & compiler) : compiler(compiler) {
	// Initialize the LLVM-specific constructs.
	this->context = new llvm::LLVMContext();
	this->builder = new llvm::IRBuilder<>(*this->context);
	this->llvmModule = std::make_unique<llvm::Module>("C-Minus LLVM", *this->context);
	// Generate LLVM for the C-Minus built-in functions: input/output.
	this->generateBuiltInFunDeclaration(this->compiler->getSymbolTableManager()->getCurrentScopeFromVector()->findSymbol("input"));
	this->generateBuiltInFunDeclaration(this->compiler->getSymbolTableManager()->getCurrentScopeFromVector()->findSymbol("output"));
	return;
}

LLVMHandler::~LLVMHandler() {
	return;
}

/** Define a function to generate LLVM for the input/output built-in function declarations. */
void LLVMHandler::generateBuiltInFunDeclaration(SymbolRecordPtrType const & symbolTableRecord) {
	llvm::Type * returnType;
	std::vector<llvm::Type *> functionArgs;
	llvm::FunctionType * llvmFunctionType;
	// Fill out the LLVM function return type.
	if (symbolTableRecord->returnType == CMINUS_NATIVE_TYPES::INT) {
		returnType = llvm::Type::getInt32Ty(*this->context);
	} else {
		returnType = llvm::Type::getVoidTy(*this->context);
	}
	// Fill out the LLVM function arguments, and generate LLVM function type.
	if (symbolTableRecord->numArguments > 0) {
		functionArgs = std::vector<llvm::Type *>(symbolTableRecord->numArguments, llvm::Type::getInt32Ty(*this->context));
		llvmFunctionType = llvm::FunctionType::get(returnType, functionArgs, false);
	} else {
		llvmFunctionType = llvm::FunctionType::get(returnType, false);
	}
	// Create the LLVM function.
	llvm::Function * llvmFunction = llvm::Function::Create(llvmFunctionType, 
			llvm::Function::InternalLinkage, symbolTableRecord->text, this->llvmModule.get());
	llvmFunction->print(llvm::errs());
}

/** Define a helper function to create an alloca instruction in the entry block of
	the function.  This is used for mutable variables etc. */
llvm::AllocaInst * LLVMHandler::createEntryBlockAlloca(
		llvm::Function * llvmFunction, std::string const & variableName) {
	llvm::IRBuilder<> tempBlock(&llvmFunction->getEntryBlock(), llvmFunction->getEntryBlock().begin());
	return tempBlock.CreateAlloca(llvm::Type::getInt32Ty(*this->context), 0, variableName.c_str());
}

/** Generate LLVM for a variable declaration node. */
void LLVMHandler::generateVarDeclaration(AstNode * const & curAstNode) {
	// Get the enclosing LLVM function object.
	llvm::Function * enclosingFunction = this->builder->GetInsertBlock()->getParent();
	// Create an alloca for the variable in the entry block.
	llvm::AllocaInst * allocaVar =
		this->createEntryBlockAlloca(enclosingFunction, curAstNode->symbolTableRecord->text);
	// Initialize this variable to 0 in LLVM.
	llvm::Value * initVal = llvm::ConstantInt::get(*this->context, llvm::APInt(32, 0, true));
	// Store the initial value into the alloca.
	this->builder->CreateStore(initVal, allocaVar);
	// Update the new LLVM variable value in the corresponding symbol table entry.
	this->compiler->getSymbolTableManager()->getCurrentScopeFromVector()->findSymbol(
			allocaVar->getName())->llvmValue = allocaVar;
}

/** Generate LLVM for a function declaration node. */
llvm::Function * LLVMHandler::generateFunDeclaration(AstNode * const & curAstNode) {
	llvm::Type * returnType;
	std::vector<llvm::Type *> functionArgs;
	llvm::FunctionType * llvmFunctionType;
	// Fill out the LLVM function return type.
	if (curAstNode->symbolTableRecord->returnType == CMINUS_NATIVE_TYPES::INT) {
		returnType = llvm::Type::getInt32Ty(*this->context);
	} else {
		returnType = llvm::Type::getVoidTy(*this->context);
	}
	// Fill out the LLVM function arguments, and generate LLVM function type.
	if (curAstNode->symbolTableRecord->numArguments > 0) {
		functionArgs = std::vector<llvm::Type *>(
				curAstNode->symbolTableRecord->numArguments, llvm::Type::getInt32Ty(*this->context));
		llvmFunctionType = llvm::FunctionType::get(returnType, functionArgs, false);
	} else {
		llvmFunctionType = llvm::FunctionType::get(returnType, false);
	}
	// Create the LLVM function.
	llvm::Function * llvmFunction = llvm::Function::Create(
			llvmFunctionType, llvm::Function::InternalLinkage, curAstNode->symbolTableRecord->text,
			this->llvmModule.get());
	// Set names for all arguments.
	unsigned int Idx = 0;
	for (auto & curArg : llvmFunction->args()) {
		// Get the params child, and loop through each function parameter.
		curArg.setName(curAstNode->children.at(0)->children.at(Idx++)->symbolTableRecord->text);
	}
	// Create a new basic block to start insertion into.
	llvm::BasicBlock * llvmBasicBlock = llvm::BasicBlock::Create(*this->context, "EntryBlock", llvmFunction);
	this->builder->SetInsertPoint(llvmBasicBlock);
	// Record the function parameter LLVM values in the symbol table.
	for (auto & curParam : llvmFunction->args()) {
		// Create an alloca for this variable.
		llvm::AllocaInst * allocaVar = this->createEntryBlockAlloca(llvmFunction, curParam.getName());
		// Store the initial value into the alloca.
		this->builder->CreateStore(&curParam, allocaVar);
		// Update the new LLVM argument value in the corresponding symbol table entry.
		this->compiler->getSymbolTableManager()->getCurrentScopeFromVector()->findSymbol(
				curParam.getName())->llvmValue = allocaVar;
	}
	// Generate the LLVM for the function body, and get the return value.
	if (llvm::Value * llvmReturnValue = curAstNode->children.at(1)->generateLLVM()) {
		// Finish off the function.
		this->builder->CreateRet(llvmReturnValue);
		// Validate the generated code, checking for consistency.
		llvm::verifyFunction(*llvmFunction);
		llvmFunction->print(llvm::errs());
		return llvmFunction;
	}
	// Error reading body, remove function.
	llvmFunction->eraseFromParent();
	std::cerr << "ERROR: LLVM could not be generated for function body." << std::endl;
	return nullptr;
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
	this->llvmModule->print(llvm::errs(), nullptr);
	//for (auto const & curEntry : this->llvmList) {
	//	std::cout << curEntry << std::endl;
	//}
}

