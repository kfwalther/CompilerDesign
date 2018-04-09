#ifndef COMPILER_H
#define COMPILER_H

/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: Compiler
 */

//#include <fstream>

#include "antlr4-runtime.h"
#include "TLexer.h"
#include "TParser.h"
#include "ErrorHandler.h"
#include "SemanticAnalyzer.h"

struct Compiler {

	/** Alias some commonly used types for convenience. */

	/** Define the constructors. */
	Compiler(std::string const & inputFile, std::ifstream & inputFileStream);
	
	/** Define the function to return a list of tokens from the input file. */
	void tokenizeInputFile();
	/** Define the function to parse the token stream. */
	void parseTokens();
	/** Define a function to account for syntax errors (incorrect grammar) in the input. */
	void checkForSyntaxErrors();
	/** Define the function to generate the AST from the parse tree. */
	void generateAst();
	/** Define a function to perform the semantic analysis, and decorate the symbol table and AST. */
	void performSemanticAnalysis();

	/** Define getters and setters for the compiler-related objects in this class. */
	AntlrGrammarGenerated::TParser * const getParser() const;
	ErrorHandler * const getErrorHandler() const;
	SemanticAnalyzer * const getSemanticAnalyzer();

	/** Define a debugging flag to turn on trace printing. */
	bool debuggingOn = false;

private:
	std::ifstream & inputFileStream;
	/** Define the token stream to be populate from the input file. */
	antlr4::CommonTokenStream * tokenStream;
	/** Define the parser object to handle all of the parsing operations. */
	AntlrGrammarGenerated::TParser * parser;
	/** Define the ParseTree object generated during the parse. */
	antlr4::tree::ParseTree * parseTree;
	/** Define the AST object derived from the parse tree, and decorated during semantic analysis. */
	AstNode * ast;
	/** Define the ErrorHandler object to manage the information for error messages. */
	ErrorHandler * errorHandler;
	/** Define the SemanticAnalyzer object to keep track of the usage of variable in memory. */
	SemanticAnalyzer * semanticAnalyzer;
};


#endif /* COMPILER_H */
