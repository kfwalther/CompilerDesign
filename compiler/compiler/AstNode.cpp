/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "AstNode.h"


/** Define a default constructor. */
AstNode::AstNode(std::string const & nodeType) : nodeType(nodeType)
{
	return;
};

AstNode::AstNode(antlr4::tree::TerminalNode * inputNode)
{
	return;
};

AstNode::~AstNode() {
	return;
}