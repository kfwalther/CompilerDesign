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
		this->compiler->getLLVMHandler()->generateVarDeclaration(this);
	} 
	else if (this->ruleType == CMINUS_RULE_TYPE::RuleFunDeclaration) {
		return this->compiler->getLLVMHandler()->generateFunDeclaration(this);
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
	else if (this->ruleType == CMINUS_RULE_TYPE::RuleSelectionStmt) {
		// Generate LLVM for the conditional expression in the if-statement.
		llvm::Value * llvmConditionalValue = this->children.at(0)->generateLLVM();
		if (!llvmConditionalValue) {
			return nullptr;
		}
		LLVMHandler * llvmHandler = this->compiler->getLLVMHandler();
		// Convert condition to a bool by comparing non-equal to 0.
		//llvmConditionalValue = llvmHandler->builder->CreateFCmpONE(
		//	llvmConditionalValue, llvm::ConstantInt::get(*llvmHandler->context, llvm::APInt(32,0,true)));
		// Get the LLVM function enclosing this block.
		llvm::Function * llvmEnclosingFunction = llvmHandler->builder->GetInsertBlock()->getParent();
		// Create blocks for the if-then and if-else cases.  Insert the 'then' block at the end of the function.
		llvm::BasicBlock * thenBlock = llvm::BasicBlock::Create(*llvmHandler->context, "then", llvmEnclosingFunction);
		llvm::BasicBlock * elseBlock = llvm::BasicBlock::Create(*llvmHandler->context, "else");
		llvm::BasicBlock * mergeBlock = llvm::BasicBlock::Create(*llvmHandler->context, "ifcont");
		llvmHandler->builder->CreateCondBr(llvmConditionalValue, thenBlock, elseBlock);
		// Emit then value.
		llvmHandler->builder->SetInsertPoint(thenBlock);
		// Generate LLVM for the expression in the if-then block.
		llvm::Value * llvmThenValue = this->children.at(1)->generateLLVM();
		if (!llvmThenValue) {
			return nullptr;
		}
		llvmHandler->builder->CreateBr(mergeBlock);
		// Codegen of 'Then' can change the current block, update thenBlock for the PHI.
		thenBlock = llvmHandler->builder->GetInsertBlock();
		// Emit else block.
		llvmEnclosingFunction->getBasicBlockList().push_back(elseBlock);
		llvmHandler->builder->SetInsertPoint(elseBlock);
		// Generate LLVM for the expression in the if-else block.
		llvm::Value * llvmElseValue = this->children.at(2)->generateLLVM();
		if (!llvmElseValue) {
			return nullptr;
		}
		llvmHandler->builder->CreateBr(mergeBlock);
		// Codegen of 'Else' can change the current block, update elseBlock for the PHI.
		elseBlock = llvmHandler->builder->GetInsertBlock();
		// Emit merge block.
		llvmEnclosingFunction->getBasicBlockList().push_back(mergeBlock);
		llvmHandler->builder->SetInsertPoint(mergeBlock);
		llvm::PHINode * phiNode = llvmHandler->builder->CreatePHI(llvm::Type::getInt32Ty(*llvmHandler->context), 2, "iftmp");
		phiNode->addIncoming(llvmThenValue, thenBlock);
		phiNode->addIncoming(llvmElseValue, elseBlock);
		return phiNode;
	}
	else if (this->ruleType == CMINUS_RULE_TYPE::RuleReturnStmt) {
		// Generate LLVM for the return value of the function block.
		if (!this->children.empty()) {
			this->children.at(0)->generateLLVM();
		} else {
			// Otherwise, return a null LLVM value.
			llvm::Constant::getNullValue(llvm::Type::getInt32Ty(*this->compiler->getLLVMHandler()->context));
		}
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
	else if ((this->ruleType == CMINUS_RULE_TYPE::RuleTerm) || (this->ruleType == CMINUS_RULE_TYPE::RuleAdditiveExpression)
			|| (this->ruleType == CMINUS_RULE_TYPE::RuleSimpleExpression)) {
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
			return this->compiler->getLLVMHandler()->builder->CreateSDiv(lhs, rhs, "divtmp");
		} 
		else if (this->token->getText() == "<") {
			return this->compiler->getLLVMHandler()->builder->CreateICmpSLT(lhs, rhs, "cmpLT");
		}
		else if (this->token->getText() == "<=") {
			return this->compiler->getLLVMHandler()->builder->CreateICmpSLE(lhs, rhs, "cmpLE");
		}
		else if (this->token->getText() == ">") {
			return this->compiler->getLLVMHandler()->builder->CreateICmpSGT(lhs, rhs, "cmpGT");
		}
		else if (this->token->getText() == ">=") {
			return this->compiler->getLLVMHandler()->builder->CreateICmpSGE(lhs, rhs, "cmpGE");
		}
		else if (this->token->getText() == "==") {
			return this->compiler->getLLVMHandler()->builder->CreateICmpEQ(lhs, rhs, "cmpEQ");
		}
		else if (this->token->getText() == "!=") {
			return this->compiler->getLLVMHandler()->builder->CreateICmpNE(lhs, rhs, "cmpNE");
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
		// TODO: Need to register input() and output() functions with llvmModule so we dont crash here.
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
		// Create the LLVM CallInst object.
		if (CalleeF->getReturnType()->isVoidTy()) {
			return this->compiler->getLLVMHandler()->builder->CreateCall(CalleeF, ArgsV, "");
		} else {
			return this->compiler->getLLVMHandler()->builder->CreateCall(CalleeF, ArgsV, "calltmp");
		}
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
