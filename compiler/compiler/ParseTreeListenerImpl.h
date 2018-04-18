/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: ParseTreeListenerImpl
 */

#include "TParserBaseListener.h"

/** Define the ParseTreeListenerImpl class, which implements the abstract methods defined in TParserBaseListener. */
struct ParseTreeListenerImpl : public AntlrGrammarGenerated::TParserBaseListener {
	/** Define a constructor. */
	ParseTreeListenerImpl(AntlrGrammarGenerated::TParser * const parser);

	void exitRelop(AntlrGrammarGenerated::TParser::RelopContext * ctx);
	void exitAddop(AntlrGrammarGenerated::TParser::AddopContext * ctx);
	void exitMulop(AntlrGrammarGenerated::TParser::MulopContext * ctx);

	/** Call this method every time a rule is exited. */
	//void exitEveryRule(antlr4::ParserRuleContext * ctx);

private:
	AntlrGrammarGenerated::TParser * parser;
};

