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
	//if (this->ruleType == CMINUS_RULE_TYPE::RuleExpression) {

	//}
	if (this->ruleType == CMINUS_RULE_TYPE::RuleFunDeclaration) {
		// TODO: START HERE: Clean this up!
		//Function *PrototypeAST::codegen() {
		//	// Make the function type:  double(double,double) etc.
		//	std::vector<Type *> Doubles(Args.size(), Type::getDoubleTy(TheContext));
		//	FunctionType *FT =
		//		FunctionType::get(Type::getDoubleTy(TheContext), Doubles, false);

		//	Function *F =
		//		Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());

		//	// Set names for all arguments.
		//	unsigned Idx = 0;
		//	for (auto &Arg : F->args())
		//		Arg.setName(Args[Idx++]);

		//	return F;
		//}

		//Function *FunctionAST::codegen() {
		//	// First, check for an existing function from a previous 'extern' declaration.
		//	Function *TheFunction = TheModule->getFunction(Proto->getName());

		//	if (!TheFunction)
		//		TheFunction = Proto->codegen();

		//	if (!TheFunction)
		//		return nullptr;

		//	// Create a new basic block to start insertion into.
		//	BasicBlock *BB = BasicBlock::Create(TheContext, "entry", TheFunction);
		//	Builder.SetInsertPoint(BB);

		//	// Record the function arguments in the NamedValues map.
		//	NamedValues.clear();
		//	for (auto &Arg : TheFunction->args())
		//		NamedValues[Arg.getName()] = &Arg;

		//	if (Value *RetVal = Body->codegen()) {
		//		// Finish off the function.
		//		Builder.CreateRet(RetVal);

		//		// Validate the generated code, checking for consistency.
		//		verifyFunction(*TheFunction);

		//		return TheFunction;
		//	}

		//	// Error reading body, remove function.
		//	TheFunction->eraseFromParent();
		//	return nullptr;
		//}
	}
	else if (this->ruleType == CMINUS_RULE_TYPE::RuleTerm) {
		auto lhs = this->children.at(0)->generateLLVM();
		auto rhs = this->children.at(1)->generateLLVM();
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
