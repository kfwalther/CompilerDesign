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
	auto tManager = this->compiler->getSymbolTableManager();
	auto scope = tManager->getCurrentScopeFromVector();
	auto inputFunctionSymbol = scope->findSymbol("input");
	if (inputFunctionSymbol) {
		this->generateBuiltInFunDeclaration(inputFunctionSymbol);
	}
	auto outputFunctionSymbol = this->compiler->getSymbolTableManager()->getCurrentScopeFromVector()->findSymbol("output");
	if (outputFunctionSymbol) {
		this->generateBuiltInFunDeclaration(outputFunctionSymbol);
	}
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
			llvm::Function::ExternalLinkage, symbolTableRecord->text, this->llvmModule.get());
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
	// Reset the return value encountered flag.
	this->returnStatementEncountered = false;
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
			llvmFunctionType, llvm::Function::ExternalLinkage, curAstNode->symbolTableRecord->text,
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
		// Set the LLVM return value, if a return value was encountered processing the function body.
		if (this->returnStatementEncountered) {
			this->builder->CreateRet(llvmReturnValue);
		} else {
			this->builder->CreateRet(nullptr);
		}
		// Validate the generated code, checking for consistency.
		llvm::verifyFunction(*llvmFunction);
		return llvmFunction;
	}
	// Error reading body, remove function.
	llvmFunction->eraseFromParent();
	std::cerr << "ERROR: LLVM could not be generated for function body." << std::endl;
	return nullptr;
}

llvm::PHINode * LLVMHandler::generateSelectionStmt(AstNode * const & curAstNode) {
	// Generate LLVM for the conditional expression in the if-statement.
	llvm::Value * llvmConditionalValue = curAstNode->children.at(0)->generateLLVM();
	if (!llvmConditionalValue) {
		return nullptr;
	}
	LLVMHandler * llvmHandler = this->compiler->getLLVMHandler();
	// Convert condition to a bool by comparing non-equal to 0.
	//llvmConditionalValue = this->builder->CreateFCmpONE(
	//	llvmConditionalValue, llvm::ConstantInt::get(*this->context, llvm::APInt(32,0,true)));
	// Get the LLVM function enclosing this block.
	llvm::Function * llvmEnclosingFunction = this->builder->GetInsertBlock()->getParent();
	// Create blocks for the if-then and if-else cases.  Insert the 'then' block at the end of the function.
	llvm::BasicBlock * thenBlock = llvm::BasicBlock::Create(*this->context, "then", llvmEnclosingFunction);
	llvm::BasicBlock * elseBlock = llvm::BasicBlock::Create(*this->context, "else");
	llvm::BasicBlock * mergeBlock = llvm::BasicBlock::Create(*this->context, "ifcont");
	this->builder->CreateCondBr(llvmConditionalValue, thenBlock, elseBlock);
	// Emit then value.
	this->builder->SetInsertPoint(thenBlock);
	// Generate LLVM for the expression in the if-then block.
	llvm::Value * llvmThenValue = curAstNode->children.at(1)->generateLLVM();
	if (!llvmThenValue) {
		return nullptr;
	}
	this->builder->CreateBr(mergeBlock);
	// Codegen of 'Then' can change the current block, update thenBlock for the PHI.
	thenBlock = this->builder->GetInsertBlock();
	// Emit else block.
	llvmEnclosingFunction->getBasicBlockList().push_back(elseBlock);
	this->builder->SetInsertPoint(elseBlock);
	// Generate LLVM for the expression in the if-else block.
	if (curAstNode->children.size() <= 2) {
		return nullptr;
	}
	llvm::Value * llvmElseValue = curAstNode->children.at(2)->generateLLVM();
	if (!llvmElseValue) {
		return nullptr;
	}
	this->builder->CreateBr(mergeBlock);
	// Codegen of 'Else' can change the current block, update elseBlock for the PHI.
	elseBlock = this->builder->GetInsertBlock();
	// Emit merge block.
	llvmEnclosingFunction->getBasicBlockList().push_back(mergeBlock);
	this->builder->SetInsertPoint(mergeBlock);
	llvm::PHINode * phiNode = this->builder->CreatePHI(llvm::Type::getInt32Ty(*this->context), 2, "iftmp");
	phiNode->addIncoming(llvmThenValue, thenBlock);
	phiNode->addIncoming(llvmElseValue, elseBlock);
	return phiNode;
}

llvm::Value * LLVMHandler::generateReturnStmt(AstNode * const & curAstNode) {
	// Generate LLVM for the return value of the function block.
	if (!curAstNode->children.empty()) {
		this->returnStatementEncountered = true;
		return curAstNode->children.at(0)->generateLLVM();
	} else {
		// Otherwise, return a null LLVM value.
		return llvm::Constant::getNullValue(llvm::Type::getInt32Ty(*this->context));
	}
}

llvm::Value * LLVMHandler::generateExpression(AstNode * const & curAstNode) {
	// Generate the LLVM for the right-hand-side expression.
	auto rhsVal = curAstNode->children.at(1)->generateLLVM();
	// Look up the variable name in the symbol table.
	llvm::Value * llvmVariable = this->compiler->getSymbolTableManager()->getCurrentScopeFromVector()->findSymbol(
			curAstNode->children.at(0)->symbolTableRecord->text)->llvmValue;
	// Store the LLVM value assigned to this LLVM variable.
	this->builder->CreateStore(rhsVal, llvmVariable);
	return rhsVal;
}

llvm::Value * LLVMHandler::generateMathExpression(AstNode * const & curAstNode) {
	// Generate LLVM for the operands.
	auto lhs = curAstNode->children.at(0)->generateLLVM();
	auto rhs = curAstNode->children.at(1)->generateLLVM();
	// Look up the operator and generate LLVM for it.
	if (curAstNode->token->getText() == "+") {
		return this->builder->CreateAdd(lhs, rhs, "addtmp");
	}
	else if (curAstNode->token->getText() == "-") {
		return this->builder->CreateSub(lhs, rhs, "subtmp");
	}
	else if (curAstNode->token->getText() == "*") {
		return this->builder->CreateMul(lhs, rhs, "multmp");
	}
	else if (curAstNode->token->getText() == "/") {
		return this->builder->CreateSDiv(lhs, rhs, "divtmp");
	}
	else if (curAstNode->token->getText() == "<") {
		return this->builder->CreateICmpSLT(lhs, rhs, "cmpLT");
	}
	else if (curAstNode->token->getText() == "<=") {
		return this->builder->CreateICmpSLE(lhs, rhs, "cmpLE");
	}
	else if (curAstNode->token->getText() == ">") {
		return this->builder->CreateICmpSGT(lhs, rhs, "cmpGT");
	}
	else if (curAstNode->token->getText() == ">=") {
		return this->builder->CreateICmpSGE(lhs, rhs, "cmpGE");
	}
	else if (curAstNode->token->getText() == "==") {
		return this->builder->CreateICmpEQ(lhs, rhs, "cmpEQ");
	}
	else if (curAstNode->token->getText() == "!=") {
		return this->builder->CreateICmpNE(lhs, rhs, "cmpNE");
	}
}

llvm::Value * LLVMHandler::generateVar(AstNode * const & curAstNode) {
	// Look this variable up in current function scope.
	if (!curAstNode->symbolTableRecord) {
		std::cerr << "ERROR: LLVM: Unknown variable name." << std::endl;
		return nullptr;
	}
	llvm::Value * curVariable = this->compiler->getSymbolTableManager()->getCurrentScopeFromVector()->findSymbol(
			curAstNode->symbolTableRecord->text)->llvmValue;
	if (!curVariable) {
		std::cerr << "ERROR: LLVM: Unknown variable name: " << curAstNode->symbolTableRecord->text << std::endl;
		return nullptr;
	}
	// Load the value.
	return this->builder->CreateLoad(curVariable, curAstNode->symbolTableRecord->text.c_str());
}

llvm::Value * LLVMHandler::generateCall(AstNode * const & curAstNode) {
	// Look up the name in the global module table.
	llvm::Function * calleeF = this->llvmModule->getFunction(curAstNode->symbolTableRecord->text);
	if (!calleeF) {
		std::cerr << "ERROR: LLVM: Unknown function referenced" << std::endl;
		return nullptr;
	}
	// If argument mismatch error.
	if (calleeF->arg_size() != curAstNode->symbolTableRecord->numArguments) {
		std::cerr << "ERROR: LLVM: Incorrect # arguments passed" << std::endl;
		return nullptr;
	}
	std::vector<llvm::Value *> argsVector;
	for (unsigned i = 0, e = curAstNode->children.size(); i != e; ++i) {
		argsVector.push_back(curAstNode->children.at(i)->generateLLVM());
		if (!argsVector.back()) {
			return nullptr;
		}
	}
	// Create the LLVM CallInst object.
	if (calleeF->getReturnType()->isVoidTy()) {
		return this->builder->CreateCall(calleeF, argsVector, "");
	} else {
		return this->builder->CreateCall(calleeF, argsVector, "calltmp");
	}
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

