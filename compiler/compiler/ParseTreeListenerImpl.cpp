/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "ParseTreeListenerImpl.h"

/** Define the default constructor for the Parse Tree Listener. */
ParseTreeListenerImpl::ParseTreeListenerImpl(AntlrGrammarGenerated::TParser * const parser) : parser(parser) {
	return;
}

/** Add the NUM/ID tokens to the symbol table. */
void ParseTreeListenerImpl::exitEveryRule(antlr4::ParserRuleContext * ctx) {
	typedef std::vector<antlr4::tree::TerminalNode *> TerminalNodePtrVectorType;
	TerminalNodePtrVectorType numTokenNodes = ctx->getTokens(this->parser->NUM);
	TerminalNodePtrVectorType idTokenNodes = ctx->getTokens(this->parser->ID);
	// Concatenate these vectors.
	numTokenNodes.insert(numTokenNodes.end(), idTokenNodes.begin(), idTokenNodes.end());
	// If there were any tokens in the matched rules of type NUM or ID, add them to the symbol table.
	if (numTokenNodes.size() > 0) {
		for (auto const & node : numTokenNodes) {
			this->parser->getSymbolTable()->insertSymbol(std::make_shared<SymbolRecord>(node->getSymbol()));
		}
	}
}

/** Add any relative operators to the symbol table. */
void ParseTreeListenerImpl::exitRelop(AntlrGrammarGenerated::TParser::RelopContext * ctx) {
	this->parser->getSymbolTable()->insertSymbol(std::make_shared<SymbolRecord>(
			dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children.at(0))->getSymbol()));
}

/** Add any additive operators to the symbol table. */
void ParseTreeListenerImpl::exitAddop(AntlrGrammarGenerated::TParser::AddopContext * ctx) {
	this->parser->getSymbolTable()->insertSymbol(std::make_shared<SymbolRecord>(
		dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children.at(0))->getSymbol()));
}

/** Add any multiplicative operators to the symbol table. */
void ParseTreeListenerImpl::exitMulop(AntlrGrammarGenerated::TParser::MulopContext * ctx) {
	this->parser->getSymbolTable()->insertSymbol(std::make_shared<SymbolRecord>(
		dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children.at(0))->getSymbol()));
}



