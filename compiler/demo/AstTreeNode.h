#ifndef ASTTREENODE_H
#define ASTTREENODE_H

/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "ParseTree.h"

/** TODO: Remove or rename this namespace constraint, it conflicts other of same name. */
struct AstTreeNode : public antlr4::tree::ParseTree {
	/** Define a constructor. */
	AstTreeNode(antlr4::tree::ParseTree * const parseTreeNode);

private:
	
};

// TODO: Create a wrapper around this type, or make Terminal node wrapper to contruct AST.

#endif ASTTREENODE_H