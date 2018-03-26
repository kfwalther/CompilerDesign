/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "TParserBaseListener.h"

/** TODO: Remove or rename this namespace constraint, it conflicts other of same name. */
struct ParseTreeListenerImpl : public AntlrGrammarGenerated::TParserBaseListener {
	/** Define a constructor. */
	ParseTreeListenerImpl(AntlrGrammarGenerated::TParser * const parser);

	void enterProgram(AntlrGrammarGenerated::TParser::ProgramContext * ctx);
	void exitProgram(AntlrGrammarGenerated::TParser::ProgramContext * ctx);

	/** Call this method every time a rule is exited. */
	void exitEveryRule(antlr4::ParserRuleContext * ctx);

private:
	AntlrGrammarGenerated::TParser * parser;
};

// TODO: Create a wrapper around this type, or make Terminal node wrapper to contruct AST.

