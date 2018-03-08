/**
 * @author: Kevin Walther
 */

#include "AstNode.hpp"

/** Define the default constructor for AstNode. */
AstNode::AstNode() : childNodeList() {
	return;
}

/** Define the default constructor for AstNode. */
AstNode::AstNode(AbstractSyntaxTreeNodeType const & newAstNodeType, AstNodePtrListType astNodePtrList) :
		nodeType(newAstNodeType) {
	return;
}

