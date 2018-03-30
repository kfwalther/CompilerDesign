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

	antlrcpp::Any visitProgram(AntlrGrammarGenerated::TParser::ProgramContext *ctx);
	antlrcpp::Any visitDeclarationList(AntlrGrammarGenerated::TParser::DeclarationListContext *ctx);
	antlrcpp::Any visitVarDeclaration(AntlrGrammarGenerated::TParser::VarDeclarationContext * ctx);
	antlrcpp::Any visitFunDeclaration(AntlrGrammarGenerated::TParser::FunDeclarationContext * ctx);
	antlrcpp::Any visitTypeSpecifier(AntlrGrammarGenerated::TParser::TypeSpecifierContext * ctx);
	antlrcpp::Any visitParams(AntlrGrammarGenerated::TParser::ParamsContext * ctx);
	antlrcpp::Any visitParamList(AntlrGrammarGenerated::TParser::ParamListContext * ctx);
	antlrcpp::Any visitParam(AntlrGrammarGenerated::TParser::ParamContext * ctx);


private:
	AntlrGrammarGenerated::TParser * parser;
};

// TODO: Create a wrapper around this type, or make Terminal node wrapper to contruct AST.



#endif PARSETREEVISITORIMPL_H