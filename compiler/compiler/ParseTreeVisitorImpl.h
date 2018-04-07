#ifndef PARSETREEVISITORIMPL_H
#define PARSETREEVISITORIMPL_H

/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: ParseTreeVisitorImpl
 */

#include "TParserBaseVisitor.h"

 /** Define the ParseTreeVisitorImpl class, which implements the abstract methods defined in TParserBaseVisitor. */
struct ParseTreeVisitorImpl : public AntlrGrammarGenerated::TParserBaseVisitor {
	/** Define a constructor. */
	ParseTreeVisitorImpl(AntlrGrammarGenerated::TParser * const parser);

	/** Define a helper function to support parse tree traversal of list nodes. */
	//TODO: These are templated, move the whole function into header!
	template<class EntityType, class EntityListType>
	AstNode::AstNodePtrVectorType populateChildrenFromList(EntityListType * ctx) {
		// Declare a vector of AstNodes to return.
		AstNode::AstNodePtrVectorType entityNodeVector;
		// Loop through all children (grammar is: Entity | EntityList Entity).
		for (auto const & curNode : ctx->children) {
			// If the current node is just a entity, add it to the entityNodeVector.
			if (antlrcpp::is<EntityType *>(curNode)) {
				// Visit the child first to create a node for specific type of entity.
				entityNodeVector.push_back(this->visit(curNode));
			}
			// Append the entity nodes from the nested EntityList to entityNodeVector.
			if (antlrcpp::is<EntityListType *>(curNode)) {
				AstNode::AstNodePtrVectorType otherEntityNodeVector = this->visit(curNode);
				// TODO: Figure out if this is the correct order (prepend or append?).
				entityNodeVector.insert(entityNodeVector.end(), otherEntityNodeVector.begin(), otherEntityNodeVector.end());
			}
		}
		return entityNodeVector;
	}

	/** Define a helper function to visit and collapse the expression/term nodes of the parse tree into the AST. */
	// TODO: May not want to collapse here to preserve order of expressions?
	template<class EntityType>
	AstNode * visitAndCollapseExpressionNodes(EntityType * ctx) {
		AstNode * curExpNode = new AstNode(ctx);
		// Check if this has a relative/additive/mult operation to process.
		if (ctx->children.size() > 1) {
			// Get the specific operator.
			curExpNode->token = dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children.at(1)->children.at(0))->getSymbol();
			// Save the two expressions on each side, which are the operands.
			curExpNode->children.push_back(this->visit(ctx->children.at(0)));
			curExpNode->children.push_back(this->visit(ctx->children.at(2)));
		}
		else {
			// Visit the child expression node, saving it in place of this node.
			curExpNode = this->visitChildren(ctx);
		}
		// Set the parents of this node's children.
		this->updateParents(curExpNode);
		return curExpNode;
	}

	/** Update information about the input() and output() functions in the symbol table. */
	void populateLanguageSpecificFunctionInfo();

	/** Define a helper function to update the parent attribute of each child node. */
	void updateParents(AstNode * const & curNode);

	/** Check if the actual parameters (arguments) here match type composition of formal parameters in function declaration. */
	bool validateFunctionParameterTypes(AstNode * const & callNode) const;

	antlrcpp::Any visitProgram(AntlrGrammarGenerated::TParser::ProgramContext * ctx);
	antlrcpp::Any visitDeclarationList(AntlrGrammarGenerated::TParser::DeclarationListContext *ctx);
	antlrcpp::Any visitVarDeclaration(AntlrGrammarGenerated::TParser::VarDeclarationContext * ctx);
	antlrcpp::Any visitFunDeclaration(AntlrGrammarGenerated::TParser::FunDeclarationContext * ctx);
	antlrcpp::Any visitTypeSpecifier(AntlrGrammarGenerated::TParser::TypeSpecifierContext * ctx);
	antlrcpp::Any visitParams(AntlrGrammarGenerated::TParser::ParamsContext * ctx);
	antlrcpp::Any visitParamList(AntlrGrammarGenerated::TParser::ParamListContext * ctx);
	antlrcpp::Any visitParam(AntlrGrammarGenerated::TParser::ParamContext * ctx);
	antlrcpp::Any visitCompoundStmt(AntlrGrammarGenerated::TParser::CompoundStmtContext * ctx);
	antlrcpp::Any visitLocalDeclaration(AntlrGrammarGenerated::TParser::LocalDeclarationContext * ctx);
	antlrcpp::Any visitStatementList(AntlrGrammarGenerated::TParser::StatementListContext * ctx);

	antlrcpp::Any visitExpressionStmt(AntlrGrammarGenerated::TParser::ExpressionStmtContext * ctx);
	antlrcpp::Any visitSelectionStmt(AntlrGrammarGenerated::TParser::SelectionStmtContext * ctx);
	antlrcpp::Any visitIterationStmt(AntlrGrammarGenerated::TParser::IterationStmtContext * ctx);
	antlrcpp::Any visitReturnStmt(AntlrGrammarGenerated::TParser::ReturnStmtContext * ctx);

	antlrcpp::Any visitExpression(AntlrGrammarGenerated::TParser::ExpressionContext * ctx);
	antlrcpp::Any visitVar(AntlrGrammarGenerated::TParser::VarContext * ctx);
	antlrcpp::Any visitSimpleExpression(AntlrGrammarGenerated::TParser::SimpleExpressionContext * ctx);
	antlrcpp::Any visitAdditiveExpression(AntlrGrammarGenerated::TParser::AdditiveExpressionContext * ctx);
	antlrcpp::Any visitTerm(AntlrGrammarGenerated::TParser::TermContext * ctx);
	antlrcpp::Any visitFactor(AntlrGrammarGenerated::TParser::FactorContext * ctx);
	antlrcpp::Any visitCall(AntlrGrammarGenerated::TParser::CallContext * ctx);
	antlrcpp::Any visitArgs(AntlrGrammarGenerated::TParser::ArgsContext * ctx);
	antlrcpp::Any visitArgList(AntlrGrammarGenerated::TParser::ArgListContext * ctx);


private:
	AntlrGrammarGenerated::TParser * parser;
};


#endif /* PARSETREEVISITORIMPL_H */