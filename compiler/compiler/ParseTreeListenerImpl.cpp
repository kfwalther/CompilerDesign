/**
 * @author: Kevin Walther
 * @date: 2018
 */

#include "ParseTreeListenerImpl.h"
//#include "SymbolTable.h"

/** Define the default constructor for the Parse Tree Listener. */
ParseTreeListenerImpl::ParseTreeListenerImpl(AntlrGrammarGenerated::TParser * const parser) : parser(parser) {
	return;
}

void ParseTreeListenerImpl::enterProgram(AntlrGrammarGenerated::TParser::ProgramContext * ctx) {
	std::cout << "Attempting parse of the program rule..." << std::endl;
}

void ParseTreeListenerImpl::exitProgram(AntlrGrammarGenerated::TParser::ProgramContext * ctx) {
	std::cout << "Completed parsing a program rule..." << std::endl;
}

void ParseTreeListenerImpl::exitEveryRule(antlr4::ParserRuleContext * ctx) {
	typedef std::vector<antlr4::tree::TerminalNode *> TerminalNodePtrVectorType;
	TerminalNodePtrVectorType numTokenNodes = ctx->getTokens(this->parser->NUM);
	TerminalNodePtrVectorType idTokenNodes = ctx->getTokens(this->parser->ID);
	// Concatenate these vectors.
	numTokenNodes.insert(numTokenNodes.end(), idTokenNodes.begin(), idTokenNodes.end());

	/** If there were any tokens in the matched rules of type NUM or ID, add them to the symbol table. */
	if (numTokenNodes.size() > 0) {
		for (auto const & node : numTokenNodes) {
			// TODO: Provide this 'node' to the SymbolRecord constructor as a pointer. std::move?
			this->parser->getSymbolTable()->insertSymbol(std::make_shared<SymbolRecord>(node->getSymbol()));
		}
	}
	// TODO: Return a pointer to symbolTable entry so this tree node can have a reference to it.

	/** Build AST here. */
	//this->parser->abstractSyntaxTree->
}


/** Define the default constructor for the Parse Tree Listener.
ParseTreeListenerImpl::ParseTreeListenerImpl() {
	return;
}
*/


