/**
* @author: Kevin Walther
* @date: 2018
* @brief: This file is the main entry point to the C-Minus compiler program.
*/

#include <iostream>

#include "Compiler.h"

/** Define a function to perform all of the actions of a compiler. */
void compileInputFile(std::ifstream & inputStream) {
	try {
		// Create the Compiler class to manage the objects used during compilation.
		std::unique_ptr<Compiler> cMinusCompiler = std::make_unique<Compiler>(inputStream);
		//cMinusCompiler->debuggingOn = true;
		// Generate a list of tokens from the input file. 
		cMinusCompiler->tokenizeInputFile();
		// Parse the tokens to populate the symbol table and generate the parse tree.
		cMinusCompiler->parseTokens();
		// Create the AST from the parse tree.
		cMinusCompiler->generateAst();
		// Walk the AST to complete semantic analysis.
		cMinusCompiler->performSemanticAnalysis();
	} catch (std::invalid_argument & invArgException) {
		std::cerr << invArgException.what() << std::endl;
	}
}

int main(int numArguments, char const * const arguments[]) {
	/** Specify the input file to read. */
	std::string inputFile("C:/Users/walther/CompilerDesign/compiler/tests/TestCase6.txt");
	std::string inputFileDesktop("D:/workspace/CompilerDesign/compiler/tests/TestCase6.txt");
	std::ifstream inputStream(inputFile);
	std::ifstream inputStreamDesktop(inputFileDesktop);
	if (inputStream.is_open()) {
		compileInputFile(inputStream);
	}
	else if (inputStreamDesktop.is_open()) {
		compileInputFile(inputStream);
	}
	else {
		std::cerr << ("ERROR: Problem opening the provided input file with std::ifstream: " + inputFile) << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
