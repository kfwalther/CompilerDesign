/**
 * @author: Kevin Walther
 * @date: 2018
 */


#include "AstNode.h"

/** Define a default constructor. */
AstNode::AstNode(CMINUS_RULE_TYPE const ruleType) : ruleType(ruleType)
{
	return;
};

AstNode::AstNode(antlr4::tree::ParseTree * inputNode)
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

std::string AstNode::PrintTreeString() {
	//std::string temp = antlrcpp::escapeWhitespace(antlr4::tree::Trees::getNodeText(t, ruleNames), false);
	//if (t->children.empty()) {
	//	return temp;
	//}

	//std::stringstream ss;
	//ss << "(" << temp << ' ';

	//// Implement the recursive walk as iteration to avoid trouble with deep nesting.
	//std::stack<size_t> stack;
	//size_t childIndex = 0;
	//ParseTree *run = t;
	//while (childIndex < run->children.size()) {
	//	if (childIndex > 0) {
	//		ss << ' ';
	//	}
	//	ParseTree *child = run->children[childIndex];
	//	temp = antlrcpp::escapeWhitespace(Trees::getNodeText(child, ruleNames), false);
	//	if (!child->children.empty()) {
	//		// Go deeper one level.
	//		stack.push(childIndex);
	//		run = child;
	//		childIndex = 0;
	//		ss << "(" << temp << " ";
	//	}
	//	else {
	//		ss << temp;
	//		while (++childIndex == run->children.size()) {
	//			if (stack.size() > 0) {
	//				// Reached the end of the current level. See if we can step up from here.
	//				childIndex = stack.top();
	//				stack.pop();
	//				run = run->parent;
	//				ss << ")";
	//			}
	//			else {
	//				break;
	//			}
	//		}
	//	}
	//}

	//ss << ")";
	//return ss.str();

}
