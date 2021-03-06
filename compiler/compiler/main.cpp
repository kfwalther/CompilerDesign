/**
* @author: Kevin Walther
* @date: 2018
* @brief: This file is the main entry point to the C-Minus compiler program.
*/

#include <iostream>

#include "Compiler.h"

/** Define a function to perform all of the actions of a compiler. */
void compileInputFile(std::string const & inputFile, std::ifstream & inputStream) {
	try {
		// Create the Compiler class to manage the objects used during compilation.
		std::shared_ptr<Compiler> cMinusCompiler = std::make_shared<Compiler>(inputFile, inputStream);
		cMinusCompiler->debuggingOn = true;
		// Generate a list of tokens from the input file. 
		cMinusCompiler->tokenizeInputFile();
		// Parse the tokens to populate the symbol table and generate the parse tree.
		cMinusCompiler->parseTokens();
		// Create the AST from the parse tree.
		cMinusCompiler->generateAst();
		// Walk the AST to complete semantic analysis and generate LLVM IR.
		cMinusCompiler->performSemanticAnalysisAndGenerateCode();
	} catch (std::invalid_argument & invArgException) {
		std::cerr << invArgException.what() << std::endl;
	}
	catch (...) {
		std::cerr << "Unknown exception occurred!" << std::endl;
	}

}

/** This is the main entry point to the program. */
int main(int numArguments, char const * const arguments[]) {
	// See if we got any input arguments (input file).
	std::string inputFileCommandLine = "";
	if (numArguments > 1) {
		inputFileCommandLine = arguments[1];
	}
	// Specify the default input file to read, if none was provided via command line.
	std::string inputFile("C-Input-1.txt");
	std::string inputFilePathLaptop("C:/Users/walther/CompilerDesign/compiler/tests/" + inputFile);
	std::string inputFilePathDesktop("D:/workspace/CompilerDesign/compiler/tests/" + inputFile);
	std::ifstream inputStreamCommandLine(inputFileCommandLine);
	std::ifstream inputStreamLaptop(inputFilePathLaptop);
	std::ifstream inputStreamDesktop(inputFilePathDesktop);
	// Check which method was used to specify the input file, and process the file.
	if (inputStreamCommandLine.is_open()) {
		compileInputFile(inputFileCommandLine, inputStreamCommandLine);
		inputFile = inputFileCommandLine;
	} else if (inputStreamLaptop.is_open()) {
		compileInputFile(inputFile, inputStreamLaptop);
	} else if (inputStreamDesktop.is_open()) {
		compileInputFile(inputFile, inputStreamDesktop);
	} else {
		std::cerr << ("ERROR: Problem opening the provided input file with std::ifstream: " + inputFile) << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
