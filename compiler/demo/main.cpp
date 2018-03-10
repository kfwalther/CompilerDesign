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
//#include <fstream>

#include "antlr4-runtime.h"
#include "TLexer.h"
#include "TParser.h"

using namespace antlrcpptest;
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

	std::cout << "Parsing the tokens based on BNF..." << std::endl;

	TParser parser(&tokens);
	/** Tell the parser to parse the input starting from a given BNF rule, in this case 'program'. */
	tree::ParseTree* tree = parser.program();
	std::cout << "Printing the parse tree..." << std::endl;
	std::cout << tree->toStringTree(&parser) << std::endl << std::endl;
}

int main(int , const char **) {

	/** Specify the input file to read. */
	std::string inputFile("D:/workspace/CompilerDesign/compiler/demo/C-Input-3.txt");
	std::ifstream inputStream(inputFile);
	if (inputStream.is_open()) {
		ParseInputFile(inputStream);
	}
	else {
		std::cerr << ("ERROR: Problem opening the provided input file with std::ifstream: " + inputFile) << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
