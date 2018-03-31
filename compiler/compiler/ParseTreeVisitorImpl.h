#ifndef PARSETREEVISITORIMPL_H
#define PARSETREEVISITORIMPL_H

/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "TParserBaseVisitor.h"

/** TODO: Remove or rename this namespace constraint, it conflicts other of same name. */
struct ParseTreeVisitorImpl : public AntlrGrammarGenerated::TParserBaseVisitor {
	/** Define a constructor. */
	ParseTreeVisitorImpl(AntlrGrammarGenerated::TParser * const parser);

	/** Define a helper function to support parse tree traversal of list nodes. */
	template<class EntityType, class EntityListType>
	AstNode::AstNodePtrVectorType populateChildrenFromList(EntityListType * ctx);

	/** Define a helper function to visit and collapse the expression/term nodes of the parse tree into the AST. */
	template<class EntityType>
	AstNode * visitAndCollapseExpressionNodes(EntityType * ctx);

	/** Define a helper function to update the parent attribute of each child node. */
	void updateParents(AstNode * const & curNode);

	antlrcpp::Any visitProgram(AntlrGrammarGenerated::TParser::ProgramContext *ctx);
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


private:
	AntlrGrammarGenerated::TParser * parser;
};

// TODO: Create a wrapper around this type, or make Terminal node wrapper to contruct AST.



#endif PARSETREEVISITORIMPL_H