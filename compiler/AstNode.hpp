/**
 * @author: Kevin Walther
 */

#include <string>
#include <memory>
#include <map>
#include "SymbolTable.hpp"

enum class AbstractSyntaxTreeNodeType {


};

/** Define an object to represent a generic node on an Abstract Syntax Tree. */
struct AstNode {
	/** Alias the types used with this class. */
	typedef std::unique_ptr<AstNode> AstNodePtrType;
	
	/** Define constructors. */
	AstNode();
	AstNode(AbstractSyntaxTreeNodeType const & newAstNodeType);

	/** Define the type of tree node this is. */
	AbstractSyntaxTreeNodeType nodeType;
	/** Define the left and right nodes of this node. */
	AstNodePtrType leftNode;
	AstNodePtrType rightNode;
};

/** Define a AST node for IDs. */
struct AstIdNode : public AstNode {
	/** Alias the types used with this class. */
	typedef int NodeValueType;

	/** Define constructors. */
	AstNode();
	AstNode(AbstractSyntaxTreeNodeType const & newAstNodeType);

	/** Define its calculated value. */
	NodeValueType calculatedValue;
	/** Define the symbol in the symbol table this node refers to. */
	SymbolRecord::SymbolRecordPtrType symbol;
};
