/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include <utility>

#include "AstNode.h"
#include "tree/Trees.h"
#include "TParser.h"

/** Define a default constructor. */
AstNode::AstNode(CMINUS_RULE_TYPE const ruleType) : ruleType(ruleType)
{
	return;
};

AstNode::AstNode(antlr4::tree::ParseTree * inputNode) : token(NULL)
{
	// Initialize some members. 
	this->initialize(inputNode);
	return;
};

AstNode::AstNode(antlr4::tree::ParseTree * inputNode, AstNodePtrType const & parentNode) : token(NULL), parent(parentNode)
{
	// Initialize some members
	this->initialize(inputNode);
	return;
}

/** Define a copy constructor. */
AstNode::AstNode(AstNodePtrType const & otherAstNode) :
		parent(otherAstNode->parent), children(otherAstNode->children), token(otherAstNode->token),
		symbolTableRecord(otherAstNode->symbolTableRecord), ruleType(otherAstNode->ruleType), tokenType(otherAstNode->tokenType) 
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

/** Define a function to return the string representation of this node. */
std::string const AstNode::getString() const {
	if (this->hasToken()) {
		if (this->symbolTableRecord != nullptr) {
			return this->symbolTableRecord->token->getText();
		} else {
			return std::move("TokenText");
		}
	}
	else {
		return ParseTreeRuleNames[static_cast<size_t>(this->ruleType)];
	}
}

std::string AstNode::printTreeString() {
	// Get the string for the current node. 
	std::string temp = this->getString();
	// Check if there are any children to deal with. 
	if (this->children.empty()) {
		return temp;
	}

	// Define the string to build and return. 
	std::string treeString;
	treeString = "(" + temp + ' ';

	// Implement the recursive walk as iteration to avoid trouble with deep nesting.
	std::stack<size_t> nodeStack;
	size_t childIndex = 0;
	AstNodePtrType curNode = this;
	// Loop through all of this node's children. 
	while (childIndex < curNode->children.size()) {
		if (childIndex > 0) {
			treeString += ' ';
		}
		AstNodePtrType child = curNode->children[childIndex];
		temp = child->getString();
		// If that child has children... 
		if (!child->children.empty()) {
			// Push the current node context onto stack, and go deeper one level. 
			nodeStack.push(childIndex);
			curNode = child;
			childIndex = 0;
			treeString += "(" + temp + " ";
		}
		else {
			treeString += temp;
			while (++childIndex == curNode->children.size()) {
				// Check if the call stack is empty.
				if (nodeStack.size() > 0) {
					// Reached the end of the current level. See if we can step up from here.
					childIndex = nodeStack.top();
					nodeStack.pop();
					curNode = curNode->parent;
					treeString += ")";
				}
				else {
					break;
				}
			}
		}
	}
	treeString += ")";
	return treeString;
}
