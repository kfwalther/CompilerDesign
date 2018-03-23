/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "AstTreeNode.h"

AstTreeNode::AstTreeNode(antlr4::tree::ParseTree * const parseTreeNode) {
	
}


MyTempAstNode::MyTempAstNode(std::string const & nodeType) : nodeType(nodeType)
{
	return;
};

MyTempAstNode::~MyTempAstNode() {
	return;
}