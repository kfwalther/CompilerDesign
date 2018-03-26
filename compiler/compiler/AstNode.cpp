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

AstNode::AstNode(antlr4::tree::TerminalNode * inputNode)
{
	//inputNode->getSymbol().
	return;
};

AstNode::~AstNode() {
	return;
}