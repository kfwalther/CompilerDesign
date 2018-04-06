/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: Compiler
 */

#include "Compiler.h"
#include "ParseTreeListenerImpl.h"
#include "ParseTreeVisitorImpl.h"
#include "AstVisitorImpl.h"

using namespace AntlrGrammarGenerated;
using namespace antlr4;

/** Define a default constructor. */
Compiler::Compiler(std::ifstream & inputFileStream) : inputFileStream(inputFileStream)
{
	return;
};

/** Define getters and setters for the compiler-related objects in this class. */
TParser * const Compiler::getParser() const {
	return this->parser;
}

/** Define a function to generate a list of tokens from the input file. */
void Compiler::tokenizeInputFile() {
	/** Define the stream of tokens. */
	this->tokenStream = new CommonTokenStream(
			new TLexer(
					new antlr4::ANTLRInputStream(this->inputFileStream)));
	tokenStream->fill();
	std::cout << "*****TOKENS*****" << std::endl;
	for (auto token : tokenStream->getTokens()) {
		std::cout << token->toString() << std::endl;
	}
}

/** Define the function to parse the token stream. */
void Compiler::parseTokens() {
	// Create the parser object.
	this->parser = new TParser(this->tokenStream);
	// Initialize the symbol table associated with this parser.
	this->parser->initializeSymbolTable();
	// Define a customized parse tree listener to perform actions during the parse.
	ParseTreeListenerImpl * parseTreeListener = new ParseTreeListenerImpl(this->parser);
	this->parser->addParseListener(parseTreeListener);
	// Tell the parser to parse the tokens starting from a given BNF rule, in this case 'program'.
	this->parseTree = this->parser->program();
	std::cout << "Printing the parse tree..." << std::endl;
	std::cout << parseTree->toStringTree(this->parser) << std::endl << std::endl;
}

/** Define the function to generate the AST from the parse tree. */
void Compiler::generateAst() {
	/** Create the AST from the parse tree. */
	ParseTreeVisitorImpl * parseTreeVisitor = new ParseTreeVisitorImpl(parser);
	this->ast = parseTreeVisitor->visitProgram(dynamic_cast<AntlrGrammarGenerated::TParser::ProgramContext *>(parseTree));
	std::cout << "Printing the AST..." << std::endl;
	std::cout << this->ast->printTreeString() << std::endl << std::endl;
}

/** Define a function to perform the semantic analysis, and decorate the symbol table and AST. */
void Compiler::performSemanticAnalysis() {
	// TODO: Should we make a SemanticAnalyzer class to track higher level stuff here (stack/heap usage)?
	std::cout << "Walking AST to perform remaining semantic analysis..." << std::endl;
	AstVisitorImpl * astVisitor = new AstVisitorImpl();
	this->ast = astVisitor->visitProgram(this->ast);
}



