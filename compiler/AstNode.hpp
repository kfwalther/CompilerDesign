/**
 * @author: Kevin Walther
 */

#include <string>
//#include <memory>
#include <list>
#include <map>
#include "SymbolTable.hpp"

enum class AbstractSyntaxTreeNodeType {
	Program,
	DeclarationList,
	Declaration,
	VarDeclaration,
	TypeSpecifier,
	FunDeclaration,
	Params,
	ParamList,
	Param,
	CompoundStmt,
	LocalDeclaration,
	StatementList,
	Statement,
	ExpressionStmt,
	SelectionStmt,
	IterationStmt,
	ReturnStmt,
	Expression,
	Var,
	SimpleExpression,
	Relop,
	AdditiveExpression,
	Addop,
	Term,
	Mulop,
	Factor,
	Call,
	Args,
	ArgList,
	Id,
	Num,
	IntKeyword,
	VoidKeyword,
	IfKeyword,
	ElseKeyword,
	WhileKeyword,
	ReturnKeyword
};

/** Define an object to represent a generic node on an Abstract Syntax Tree. */
struct AstNode {
	/** Alias the types used with this class. */
	typedef AstNode * AstNodePtrType;
	typedef std::list<AstNodePtrType> AstNodePtrListType;
	/** Alias the types used with this class. */
	typedef int NodeValueType;
	
	/** Define constructors. */
	AstNode();
	AstNode(AbstractSyntaxTreeNodeType const & newAstNodeType, AstNodePtrListType astNodePtrList);

	/** Define the type of tree node this is. */
	AbstractSyntaxTreeNodeType nodeType;
	/** Define the left and right nodes of this node. */
	AstNodePtrListType childNodeList;
	/** Define its calculated value. */
	NodeValueType calculatedValue;
	/** Define the symbol in the symbol table this node refers to. */
	SymbolRecord::SymbolRecordPtrType symbol;
};

/** Define a AST node for IDs. */
//struct AstIdNode : public AstNode {
//
//
//	/** Define constructors. */
//	AstNode();
//	AstNode(AbstractSyntaxTreeNodeType const & newAstNodeType);
//
//
//};


