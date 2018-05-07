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
struct AstNode;
struct Compiler;
struct Scope;
struct SymbolRecord;

struct LLVMHandler {

	/** Alias some commonly used types for convenience. */
	typedef std::string LLVMInstructionType;
	typedef std::shared_ptr<SymbolRecord> SymbolRecordPtrType;

	/** Define the constructors. */
	LLVMHandler(Compiler * const & compiler);
	~LLVMHandler();
	
	/** Define a helper function to create an alloca instruction in the entry block of
		the function.  This is used for mutable variables etc. */
	llvm::AllocaInst * createEntryBlockAlloca(llvm::Function * llvmFunction, std::string const & variableName);
	/** Define some functions to generate LLVM for various types of AstNodes. */
	void generateVarDeclaration(AstNode * const & curAstNode);
	void generateBuiltInFunDeclaration(SymbolRecordPtrType const & symbolTableRecord);
	llvm::Function * generateFunDeclaration(AstNode * const & curAstNode);
	llvm::PHINode * generateSelectionStmt(AstNode * const & curAstNode);
	llvm::Value * generateReturnStmt(AstNode * const & curAstNode);
	llvm::Value * generateExpression(AstNode * const & curAstNode);
	llvm::Value * generateMathExpression(AstNode * const & curAstNode);
	llvm::Value * generateVar(AstNode * const & curAstNode);
	llvm::Value * generateCall(AstNode * const & curAstNode);
	/** Define a function to save the LLVM Value as a string in the list. */
	void saveLLVMInstruction(llvm::Value * llvmValue);
	/** Define a function to print the generated LLVM strings. */
	void printAll();
	/** Define some LLVM-specific constructs to support IR generation. */
	llvm::LLVMContext * context;
	llvm::IRBuilder<> * builder;
	std::unique_ptr<llvm::Module> llvmModule;
	bool returnStatementEncountered = false;
	//static std::map<std::string, Value *> NamedValues;

	/** Define the list of LLVM commands to be generated. */
	std::list<LLVMInstructionType> llvmList;
	/** Define a pointer to the compiler class. */
	Compiler * compiler;
};


#endif /* LLVMHANDLER_H */
