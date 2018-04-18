/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "ParseTreeListenerImpl.h"
#include "Compiler.h"
#include "SymbolTable.h"
#include "SymbolTableManager.h"
#include "Scope.h"

/** Define the default constructor for the Parse Tree Listener. */
ParseTreeListenerImpl::ParseTreeListenerImpl(AntlrGrammarGenerated::TParser * const parser) : parser(parser) {
	return;
}

/** Add any relative operators to the symbol table. */
void ParseTreeListenerImpl::exitRelop(AntlrGrammarGenerated::TParser::RelopContext * ctx) {
	// TODO: Add these to symbol table also? Why were they added in the first place?
	//this->parser->compiler->getSymbolTableForCurrentContext()->insertSymbol(
	//		std::make_shared<SymbolRecord>(dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children.at(0))->getSymbol()));
}

/** Add any additive operators to the symbol table. */
void ParseTreeListenerImpl::exitAddop(AntlrGrammarGenerated::TParser::AddopContext * ctx) {
	//this->parser->compiler->getSymbolTableForCurrentContext()->insertSymbol(
	//		std::make_shared<SymbolRecord>(dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children.at(0))->getSymbol()));
}

/** Add any multiplicative operators to the symbol table. */
void ParseTreeListenerImpl::exitMulop(AntlrGrammarGenerated::TParser::MulopContext * ctx) {
	//this->parser->compiler->getSymbolTableForCurrentContext()->insertSymbol(
	//		std::make_shared<SymbolRecord>(dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children.at(0))->getSymbol()));
}



