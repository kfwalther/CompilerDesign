/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: AstNode
 */

#include <utility>

#include "AstNode.h"
#include "SymbolTable.h"
#include "tree/Trees.h"
#include "TParser.h"
#include "Compiler.h"
#include "SymbolTableManager.h"
#include "Scope.h"
#include "LLVMHandler.h"

#include <llvm/ADT/APInt.h>
#include <llvm/IR/Verifier.h>

/** Define a default constructor. */
AstNode::AstNode(CMINUS_RULE_TYPE const ruleType) : ruleType(ruleType)
{
	return;
};

AstNode::AstNode(antlr4::tree::ParseTree * inputNode, Compiler * const & compiler) : token(NULL), compiler(compiler)
{
	// Initialize some members. 
	this->initialize(inputNode);
	return;
};

AstNode::AstNode(antlr4::tree::ParseTree * inputNode, 
		AstNodePtrType const & parentNode, Compiler * const & compiler) : token(NULL), parent(parentNode), compiler(compiler)
{
	// Initialize some members
	this->initialize(inputNode);
	return;
}

/** Define a copy constructor. */
AstNode::AstNode(AstNodePtrType const & otherAstNode) :
		parent(otherAstNode->parent), children(otherAstNode->children), token(otherAstNode->token),
		symbolTableRecord(otherAstNode->symbolTableRecord), ruleType(otherAstNode->ruleType), tokenType(otherAstNode->tokenType),
		isRValue(otherAstNode->isRValue), isLValue(otherAstNode->isLValue)
{
	return;
}

void AstNode::initialize(antlr4::tree::ParseTree * inputNode) {
	std::vector<std::string> ruleNames;
	antlr4::tree::Trees::getNodeText(inputNode, ruleNames);
	/** Save the Token, if this is a leaf node. */
	if (antlrcpp::is<antlr4::tree::TerminalNode *>(inputNode)) {
		std::cout << "Creating AstNode with token: " << this->token->getText() << std::endl;
		this->token = dynamic_cast<antlr4::tree::TerminalNode *>(inputNode)->getSymbol();
	}
	/** Save the rule type, if this is a rule context node. */
	if (antlrcpp::is<antlr4::RuleContext *>(inputNode)) {
		antlr4::RuleContext * tempRuleNode = dynamic_cast<antlr4::RuleContext *>(inputNode);
		this->ruleType = static_cast<CMINUS_RULE_TYPE>(tempRuleNode->getRuleIndex());
		// Define certain nodes as R-Values or L-Values.
		if (this->ruleType == CMINUS_RULE_TYPE::RuleVar) {
			this->isLValue = true;
		}
		else if ((this->ruleType == CMINUS_RULE_TYPE::RuleExpression) || (this->ruleType == CMINUS_RULE_TYPE::RuleSimpleExpression) ||
			(this->ruleType == CMINUS_RULE_TYPE::RuleAdditiveExpression) || (this->ruleType == CMINUS_RULE_TYPE::RuleTerm) ||
			(this->ruleType == CMINUS_RULE_TYPE::RuleFactor) || (this->ruleType == CMINUS_RULE_TYPE::RuleCall)) {
			this->isRValue = true;
		}
		// Define certain nodes as type INT upon creation.
		if ((this->ruleType == CMINUS_RULE_TYPE::RuleVarDeclaration) || (this->ruleType == CMINUS_RULE_TYPE::RuleParam) ||
			(this->ruleType == CMINUS_RULE_TYPE::RuleVar) || (this->ruleType == CMINUS_RULE_TYPE::RuleFactor)) {
			this->evaluatedType = CMINUS_NATIVE_TYPES::INT;
		}
		else {
			this->evaluatedType = CMINUS_NATIVE_TYPES::UNKNOWN;
		}
	}
}

AstNode::~AstNode() {
	return;
}

/** Define a function to check if this node is associated with a Token. */
bool AstNode::hasToken() const {
	if (this->token != NULL) {
		return true;
	}
	else {
		return false;
	}
}

/** Define a function to generate LLVM code for this AST node. */
/** The code in this function was heavily influenced by Kaleidoscope example on LLVM API site: 
	https://llvm.org/docs/tutorial/index.html */
llvm::Value * AstNode::generateLLVM() {
	if (this->ruleType == CMINUS_RULE_TYPE::RuleVarDeclaration) {
		// Get the enclosing LLVM function object.
		llvm::Function * enclosingFunction = this->compiler->getLLVMHandler()->builder->GetInsertBlock()->getParent();
		// Create an alloca for the variable in the entry block.
		llvm::AllocaInst * allocaVar = 
				this->compiler->getLLVMHandler()->createEntryBlockAlloca(enclosingFunction, this->symbolTableRecord->text);
		// Initialize this variable to 0 in LLVM.
		llvm::Value * initVal = llvm::ConstantInt::get(*this->compiler->getLLVMHandler()->context, llvm::APInt(32, 0, true));
		// Store the initial value into the alloca.
		this->compiler->getLLVMHandler()->builder->CreateStore(initVal, allocaVar);
		// Update the new LLVM variable value in the corresponding symbol table entry.
		this->compiler->getSymbolTableManager()->getCurrentScopeFromVector()->findSymbol(
				allocaVar->getName())->llvmValue = allocaVar;
	} 
	else if (this->ruleType == CMINUS_RULE_TYPE::RuleFunDeclaration) {
		llvm::Type * returnType;
		std::vector<llvm::Type *> functionArgs;
		llvm::FunctionType * llvmFunctionType;
		// Fill out the LLVM function return type.
		if (this->symbolTableRecord->returnType == CMINUS_NATIVE_TYPES::INT) {
			returnType = llvm::Type::getInt32Ty(*this->compiler->getLLVMHandler()->context);
		} else {
			returnType = llvm::Type::getVoidTy(*this->compiler->getLLVMHandler()->context);
		}
		// Fill out the LLVM function arguments, and generate LLVM function type.
		if (this->symbolTableRecord->numArguments > 0) {
			functionArgs = std::vector<llvm::Type *>(
					this->symbolTableRecord->numArguments, llvm::Type::getInt32Ty(*this->compiler->getLLVMHandler()->context));
			llvmFunctionType = llvm::FunctionType::get(returnType, functionArgs, false);
		} else {
			llvmFunctionType = llvm::FunctionType::get(returnType, false);
		}
		// Create the LLVM function.
		llvm::Function * llvmFunction = llvm::Function::Create(
				llvmFunctionType, llvm::Function::InternalLinkage, this->symbolTableRecord->text,
						this->compiler->getLLVMHandler()->llvmModule.get());
		// Set names for all arguments.
		unsigned int Idx = 0;
		for (auto & curArg : llvmFunction->args()) {
			// Get the params child, and loop through each function parameter.
			curArg.setName(this->children.at(0)->children.at(Idx)->symbolTableRecord->text);
		}
		// Create a new basic block to start insertion into.
		llvm::BasicBlock * llvmBasicBlock = llvm::BasicBlock::Create(
				*this->compiler->getLLVMHandler()->context, "EntryBlock", llvmFunction);
		this->compiler->getLLVMHandler()->builder->SetInsertPoint(llvmBasicBlock);
		// Record the function parameter LLVM values in the symbol table.
		for (auto & curParam : llvmFunction->args()) {
			// Create an alloca for this variable.
			llvm::AllocaInst * allocaVar = 
					this->compiler->getLLVMHandler()->createEntryBlockAlloca(llvmFunction, curParam.getName());
			// Store the initial value into the alloca.
			this->compiler->getLLVMHandler()->builder->CreateStore(&curParam, allocaVar);
			// Update the new LLVM argument value in the corresponding symbol table entry.
			this->compiler->getSymbolTableManager()->getCurrentScopeFromVector()->findSymbol(
					curParam.getName())->llvmValue = allocaVar;
		}
		// Generate the LLVM for the function body, and get the return value.
		// TODO: CompoundStatement generateLLVM needs to return an LLVM value.
		if (llvm::Value * llvmReturnValue = this->children.at(1)->generateLLVM()) {
			// Finish off the function.
			this->compiler->getLLVMHandler()->builder->CreateRet(llvmReturnValue);
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
	else if (this->ruleType == CMINUS_RULE_TYPE::RuleCompoundStmt) {
		// Generate LLVM for any local variable declarations in this block.
		this->children.at(0)->generateLLVM();
		// Generate LLVM and return for any statememts in this block.
		return this->children.at(1)->generateLLVM();
	}
	else if (this->ruleType == CMINUS_RULE_TYPE::RuleLocalDeclaration) {
		// Generate LLVM for each local variable declaration in this block.
		for (auto & curVarDecl : this->children) {
			curVarDecl->generateLLVM();
		}
	}
	else if (this->ruleType == CMINUS_RULE_TYPE::RuleStatementList) {
		// Generate LLVM for each local variable declaration in this block.
		// TODO: Is this the best way to return a value, if there are multiple expressions in block?
		llvm::Value * returnVal;
		for (auto & curExpression : this->children) {
			returnVal = curExpression->generateLLVM();
		}
		return returnVal;
	}
	else if (this->ruleType == CMINUS_RULE_TYPE::RuleExpression) {
		// Generate the LLVM for the right-hand-side expression.
		auto rhsVal = this->children.at(1)->generateLLVM();
		// Look up the variable name in the symbol table.
		llvm::Value * llvmVariable = this->compiler->getSymbolTableManager()->getCurrentScopeFromVector()->findSymbol(
			this->children.at(0)->symbolTableRecord->text)->llvmValue;
		// Store the LLVM value assigned to this LLVM variable.
		this->compiler->getLLVMHandler()->builder->CreateStore(rhsVal, llvmVariable);
		return rhsVal;
	}
	else if ((this->ruleType == CMINUS_RULE_TYPE::RuleTerm) || (this->ruleType == CMINUS_RULE_TYPE::RuleAdditiveExpression)) {
		// Generate LLVM for the operands.
		auto lhs = this->children.at(0)->generateLLVM();
		auto rhs = this->children.at(1)->generateLLVM();
		// Look up the operator and generate LLVM for it.
		if (this->token->getText() == "+") {
			return this->compiler->getLLVMHandler()->builder->CreateAdd(lhs, rhs, "addtmp");
		}
		else if (this->token->getText() == "-") {
			return this->compiler->getLLVMHandler()->builder->CreateSub(lhs, rhs, "subtmp");
		}
		else if (this->token->getText() == "*") {
			return this->compiler->getLLVMHandler()->builder->CreateMul(lhs, rhs, "multmp");
		}
		else if (this->token->getText() == "/") {
			return this->compiler->getLLVMHandler()->builder->CreateFDiv(lhs, rhs, "divtmp");
		}
	}
	else if (this->ruleType == CMINUS_RULE_TYPE::RuleFactor) {
		std::cout << this->symbolTableRecord->value << std::endl;
		return llvm::ConstantInt::get(*this->compiler->getLLVMHandler()->context, llvm::APInt(32, this->symbolTableRecord->value, true));
	}
	else if (this->ruleType == CMINUS_RULE_TYPE::RuleVar) {
		// Look this variable up in current function scope.
		llvm::Value * curVariable = this->compiler->getSymbolTableManager()->getCurrentScopeFromVector()->findSymbol(
				this->symbolTableRecord->text)->llvmValue;
		if (!curVariable) {
			std::cerr << "ERROR: LLVM: Unknown variable name." << std::endl;
			return nullptr;
		}
		// Load the value.
		return this->compiler->getLLVMHandler()->builder->CreateLoad(curVariable, this->symbolTableRecord->text.c_str());
	}
	else if (this->ruleType == CMINUS_RULE_TYPE::RuleCall) {
		// Look up the name in the global module table.
		llvm::Function * CalleeF = this->compiler->getLLVMHandler()->llvmModule->getFunction(this->symbolTableRecord->text);
		if (!CalleeF) {
			std::cerr << "ERROR: LLVM: Unknown function referenced" << std::endl;
			return nullptr;
		}
		// If argument mismatch error.
		if (CalleeF->arg_size() != this->symbolTableRecord->numArguments) {
			std::cerr << "ERROR: LLVM: Incorrect # arguments passed" << std::endl;
			return nullptr;
		}
		std::vector<llvm::Value *> ArgsV;
		for (unsigned i = 0, e = this->symbolTableRecord->numArguments; i != e; ++i) {
			ArgsV.push_back(this->children.at(i)->generateLLVM());
			if (!ArgsV.back()) {
				return nullptr;
			}
		}
		return this->compiler->getLLVMHandler()->builder->CreateCall(CalleeF, ArgsV, "calltmp");
	}
	else {
		return nullptr;
	}
}

/** Define a function to return the string representation of this node. */
std::string const AstNode::getString() const {
	std::string returnString = ParseTreeRuleNames[static_cast<std::size_t>(this->ruleType)];
	if (this->symbolTableRecord != nullptr) {
		returnString += (" " + this->symbolTableRecord->text) + ": " 
				+ SymbolRecordKindNames[static_cast<unsigned int>(this->symbolTableRecord->kind)] + " "
				+ CMinusNativeTypeNames[static_cast<unsigned int>(this->symbolTableRecord->type)];
	}
	return returnString;
}

/** Print the entire AST. */
std::string AstNode::printTreeString() {
	// Get the string for the current node. 
	std::string temp = this->getString();
	// Check if there are any children to deal with. 
	if (this->children.empty()) {
		return temp;
	}

	// Define the string to build and return. 
	std::string treeString;
	std::size_t indentNum = 0;
	treeString = "(" + temp + '\n';
	indentNum++;

	// Implement the recursive walk as iteration to avoid trouble with deep nesting.
	std::stack<std::size_t> nodeStack;
	std::size_t childIndex = 0;
	AstNodePtrType curNode = this;
	// Loop through all of this node's children. 
	while (childIndex < curNode->children.size()) {
		if (childIndex > 0) {
			//treeString += ' ';
		}
		AstNodePtrType child = curNode->children[childIndex];
		temp = child->getString();
		// If that child has children... 
		if (!child->children.empty()) {
			// Push the current node context onto stack, and go deeper one level. 
			nodeStack.push(childIndex);
			curNode = child;
			childIndex = 0;
			indentNum++;
			treeString += std::string(indentNum, ' ') + "(" + temp + "\n";
			indentNum++;
		}
		else {
			treeString += std::string(indentNum, ' ') + "(" + temp + ")" + "\n";
			while (++childIndex == curNode->children.size()) {
				// Check if the call stack is empty.
				if (nodeStack.size() > 0) {
					// Reached the end of the current level. See if we can step up from here.
					childIndex = nodeStack.top();
					nodeStack.pop();
					curNode = curNode->parent;
					indentNum--;
					treeString += std::string(indentNum, ' ') + ")" + "\n";
					indentNum--;
				}
				else {
					break;
				}
			}
		}
	}
	indentNum--;
	treeString += std::string(indentNum, ' ') + ")";
	return treeString;
}
