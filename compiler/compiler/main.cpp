/* Copyright (c) 2012-2017 The ANTLR Project. All rights reserved.
 * Use of this file is governed by the BSD 3-clause license that
 * can be found in the LICENSE.txt file in the project root.
 */

//
//  main.cpp
//  antlr4-cpp-demo
//
//  Created by Mike Lischke on 13.03.16.
//

#include <iostream>

#include "antlr4-runtime.h"
#include "TLexer.h"
#include "TParser.h"
#include "ParseTreeListenerImpl.h"
#include "ParseTreeVisitorImpl.h"

using namespace AntlrGrammarGenerated;
using namespace antlr4;


/** Define the function to parse the input file. */
void ParseInputFile(std::ifstream & inputStream) {
	/** Define the ANTLRInputStream object. */
	ANTLRInputStream antlrInputStream(inputStream);
	/** Define the lexer object. */
	TLexer lexer(&antlrInputStream);
	/** Define the stream of tokens. */
	CommonTokenStream tokens(&lexer);

	tokens.fill();
	std::cout << "****TOKENS*****" << std::endl;
	for (auto token : tokens.getTokens()) {
		std::cout << token->toString() << std::endl;
	}
	/** Create the parser object, and initialize the symbol table. */
	TParser parser(&tokens);
	parser.initializeSymbolTable();

	/** Define our custom listener to perform actions during the parse. */
	ParseTreeListenerImpl * parseTreeListener = new ParseTreeListenerImpl(&parser);
	parser.addParseListener(parseTreeListener);

	/** Tell the parser to parse the input starting from a given BNF rule, in this case 'program'. */
	tree::ParseTree * tree = parser.program();
	// TODO: Do we need to build AST after completing Parse Tree? 
	std::cout << "Printing the parse tree..." << std::endl;
	std::cout << tree->toStringTree(&parser) << std::endl << std::endl;

	/** TODO: Create the AST from the parse tree. */
	ParseTreeVisitorImpl * parseTreeVisitor = new ParseTreeVisitorImpl(&parser);
	AstNode * ast = parseTreeVisitor->visitProgram(dynamic_cast<AntlrGrammarGenerated::TParser::ProgramContext *>(tree));
	std::cout << "Printing the AST..." << std::endl;
	std::cout << ast->printTreeString() << std::endl << std::endl;

	/** Print the contents of the symbol table. */
	parser.getSymbolTable()->printSymbolTable();
}

int main(int numArguments, char const * const arguments[]) {

	/** Specify the input file to read. */
	std::string inputFile("C:/Users/walther/CompilerDesign/compiler/compiler/C-Input-1.txt");
	std::string inputFileDesktop("D:/workspace/CompilerDesign/compiler/compiler/C-Input-1.txt");
	std::ifstream inputStream(inputFile);
	std::ifstream inputStreamDesktop(inputFileDesktop);
	if (inputStream.is_open()) {
		ParseInputFile(inputStream);
	}
	else if (inputStreamDesktop.is_open()) {
		ParseInputFile(inputStreamDesktop);
	}
	else {
		std::cerr << ("ERROR: Problem opening the provided input file with std::ifstream: " + inputFile) << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
