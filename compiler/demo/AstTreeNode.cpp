/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "AstTreeNode.h"

//AstTreeNode::AstTreeNode(antlr4::tree::ParseTree * const parseTreeNode) {
//	
//}


AstTreeNode::AstTreeNode(std::string const & nodeType) : nodeType(nodeType)
{
	return;
};

AstTreeNode ::~AstTreeNode() {
	return;
}