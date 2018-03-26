/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "TParserBaseVisitor.h"

/** Fix for issue identified on antlr4 GitHub:
	https://github.com/antlr/antlr4/issues/2211 
	Must copy the destructor here to avoid link errors. */
antlrcpp::Any::Base::~Base() {
}

/** TODO: Remove or rename this namespace constraint, it conflicts other of same name. */
struct ParseTreeVisitorImpl : public antlrcpptest::TParserBaseVisitor {
	/** Define a constructor. */
	ParseTreeVisitorImpl(antlrcpptest::TParser * const parser);

	antlrcpp::Any visitVarDeclaration(antlrcpptest::TParser::VarDeclarationContext * ctx);
	antlrcpp::Any visitFunDeclaration(antlrcpptest::TParser::FunDeclarationContext * ctx);
private:
	antlrcpptest::TParser * parser;
};

// TODO: Create a wrapper around this type, or make Terminal node wrapper to contruct AST.

