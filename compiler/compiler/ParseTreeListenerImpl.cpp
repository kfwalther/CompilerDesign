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

/** Add the NUM/ID tokens to the symbol table. */
void ParseTreeListenerImpl::exitEveryRule(antlr4::ParserRuleContext * ctx) {
	typedef std::vector<antlr4::tree::TerminalNode *> TerminalNodePtrVectorType;
	TerminalNodePtrVectorType numTokenNodes = ctx->getTokens(this->parser->NUM);
	TerminalNodePtrVectorType idTokenNodes = ctx->getTokens(this->parser->ID);
	// If there were any tokens in the matched rules of type NUM or ID, add them to the symbol table.
	if (numTokenNodes.size() > 0) {
		std::shared_ptr<SymbolRecord> numSymbolRecord = std::make_shared<SymbolRecord>(numTokenNodes.front()->getSymbol());
		// Initialize the NUM tokens in the symbol table.
		numSymbolRecord->isDeclared = true;
		numSymbolRecord->isDefined = true;
		this->parser->compiler->getSymbolTableManager()->getCurrentScope()->scopedSymbolTable->insertSymbol(numSymbolRecord);
	}
	if (idTokenNodes.size() > 0) {
		this->parser->compiler->getSymbolTableManager()->getCurrentScope()->scopedSymbolTable->insertSymbol(
				std::make_shared<SymbolRecord>(idTokenNodes.front()->getSymbol()));
	}
}

/** Add any relative operators to the symbol table. */
void ParseTreeListenerImpl::exitRelop(AntlrGrammarGenerated::TParser::RelopContext * ctx) {
	this->parser->compiler->getSymbolTableManager()->getCurrentScope()->scopedSymbolTable->insertSymbol(
			std::make_shared<SymbolRecord>(dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children.at(0))->getSymbol()));
}

/** Add any additive operators to the symbol table. */
void ParseTreeListenerImpl::exitAddop(AntlrGrammarGenerated::TParser::AddopContext * ctx) {
	this->parser->compiler->getSymbolTableManager()->getCurrentScope()->scopedSymbolTable->insertSymbol(
			std::make_shared<SymbolRecord>(dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children.at(0))->getSymbol()));
}

/** Add any multiplicative operators to the symbol table. */
void ParseTreeListenerImpl::exitMulop(AntlrGrammarGenerated::TParser::MulopContext * ctx) {
	this->parser->compiler->getSymbolTableManager()->getCurrentScope()->scopedSymbolTable->insertSymbol(
			std::make_shared<SymbolRecord>(dynamic_cast<antlr4::tree::TerminalNode *>(ctx->children.at(0))->getSymbol()));
}



