#ifndef ASTNODE_H
#define ASTNODE_H

/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "ParseTree.h"


// TODO: Create a wrapper around this type, or make Terminal node wrapper to contruct AST.
struct AstNode {
	AstNode(std::string const & nodeType);
	~AstNode();
	AstNode * parent;
	std::vector<AstNode *> children;
	
	std::string nodeType;
};


#endif ASTNODE_H