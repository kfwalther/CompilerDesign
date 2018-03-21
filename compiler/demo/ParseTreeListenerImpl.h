/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "TParserBaseListener.h"

/** TODO: Remove or rename this namespace constraint, it conflicts other of same name. */
struct ParseTreeListenerImpl : public antlrcpptest::TParserBaseListener {
	/** Define a constructor. */
	ParseTreeListenerImpl(antlrcpptest::TParser * const parser);

	void enterProgram(antlrcpptest::TParser::ProgramContext * ctx);
	void exitProgram(antlrcpptest::TParser::ProgramContext * ctx);

	/** Call this method every time a rule is exited. */
	void exitEveryRule(antlr4::ParserRuleContext * ctx);

private:
	antlrcpptest::TParser * parser;
};

// TODO: Create a wrapper around this type, or make Terminal node wrapper to contruct AST.

