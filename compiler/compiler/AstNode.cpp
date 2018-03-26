/**
 * @author: Kevin Walther
 * @date: 2018
 */

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
	std::vector<std::string> ruleNames;
	antlr4::tree::Trees::getNodeText(inputNode, ruleNames);
	/** Save the Token, if this is a leaf node. */
	if (antlrcpp::is<antlr4::tree::TerminalNode *>(inputNode)) {
		this->token = dynamic_cast<antlr4::tree::TerminalNode *>(inputNode)->getSymbol();
	}
	/** Save the rule type, if this is a rule context node. */
	if (antlrcpp::is<antlr4::RuleContext *>(inputNode)) {
		antlr4::RuleContext * tempRuleNode = dynamic_cast<antlr4::RuleContext *>(inputNode);
		this->ruleType = static_cast<CMINUS_RULE_TYPE>(tempRuleNode->getRuleIndex());
	}
	return;
};

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
std::string const & AstNode::getString() const {
	if (this->hasToken()) {
		//return this->token->getText();
		return "TokenText";
	}
	else {
		//return ParseTreeRuleNames[static_cast<size_t>(this->ruleType)];
		return "RuleText";
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
	AstNode * curNode = this;
	// Loop through all of this node's children. 
	while (childIndex < curNode->children.size()) {
		if (childIndex > 0) {
			treeString += ' ';
		}
		AstNode * child = curNode->children[childIndex].get();
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
