/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: Compiler
 */

#include "Compiler.h"
#include "TLexer.h"
#include "SemanticAnalyzer.h"
#include "AstNode.h"
#include "SymbolTableManager.h"
#include "Scope.h"
#include "SymbolTable.h"
#include "ParseTreeListenerImpl.h"
#include "ParseTreeVisitorImpl.h"
#include "AstVisitorImpl.h"
#include "LLVMHandler.h"

using namespace AntlrGrammarGenerated;
using namespace antlr4;

/** Define a default constructor. */
Compiler::Compiler(std::string const & inputFile, std::ifstream & inputFileStream) : inputFileStream(inputFileStream)
{
	// Create the error handler for this compiler.
	this->errorHandler = new ErrorHandler(inputFile);
	return;
};

/** Define getters and setters for the compiler-related objects in this class. */
CommonTokenStream * const Compiler::getTokenStream() const {
	return this->tokenStream;
}
TParser * const Compiler::getParser() const {
	return this->parser;
}
ErrorHandler * const Compiler::getErrorHandler() const {
	return this->errorHandler;
}
SemanticAnalyzer * const Compiler::getSemanticAnalyzer() {
	return this->semanticAnalyzer;
}
SymbolTable::SymbolTablePtrType const Compiler::getSymbolTableForCurrentContext() {
	return this->getSymbolTableManager()->getCurrentScope()->scopedSymbolTable;
}
LLVMHandler * const Compiler::getLLVMHandler() const {
	return this->llvmHandler;
}

/** Define a function to generate a list of tokens from the input file. */
void Compiler::tokenizeInputFile() {
	// Define the stream of tokens.
	this->tokenStream = new CommonTokenStream(
			new TLexer(
					new ANTLRInputStream(this->inputFileStream)));
	this->tokenStream->fill();
	// Print out the token if debugging is enabled.
	if (this->debuggingOn) {
		std::cout << "*****TOKENS*****" << std::endl;
		for (auto token : this->tokenStream->getTokens()) {
			std::cout << token->toString() << std::endl;
		}
	}
}

/** Define the function to parse the token stream. */
void Compiler::parseTokens() {
	std::cout << std::endl << "Parsing tokens to generate parse tree and symbol table..." << std::endl;
	// Create the parser object.
	this->parser = new TParser(this->tokenStream);
	// Initialize the symbol table associated with this parser.
	this->parser->compiler = shared_from_this();
	this->symbolTableManager = std::make_shared<SymbolTableManager>();
	// Define a customized parse tree listener to perform actions during the parse.
	ParseTreeListenerImpl * parseTreeListener = new ParseTreeListenerImpl(this->parser);
	this->parser->addParseListener(parseTreeListener);
	// Tell the parser to parse the tokens starting from a given BNF rule, in this case 'program'.
	this->parseTree = this->parser->program();
	// Check for any syntax errors during parse, in which BNF rules were not matched by input. 
	this->checkForSyntaxErrors();
	std::cout << std::endl << "Printing the parse tree..." << std::endl;
	std::cout << std::endl << this->parseTree->toStringTree(this->parser) << std::endl << std::endl;
}

/** Define a function to account for syntax errors (incorrect grammar) in the input. */
void Compiler::checkForSyntaxErrors() {
	if (this->parser->getNumberOfSyntaxErrors()) {
		std::stringstream errorStr;
		errorStr << ErrorHandler::ErrorCodes::INVALID_SYNTAX << std::endl;
		errorStr << "Problem occurred matching input file (" + this->errorHandler->inputFile + ") syntax to expected C-Minus language grammar rules. "
				<< "Please consult the previous errors to correct the syntax issue in your program." << std::endl << std::endl;
		// If syntax errors occurred, exit the compiler program as further semantic analysis may crash.
		throw std::invalid_argument(errorStr.str());
	}
}

/** Define the function to generate the AST from the parse tree. */
void Compiler::generateAst() {
	this->semanticAnalyzer = new SemanticAnalyzer();
	std::cout << "Generating AST from the parse tree..." << std::endl;
	/** Create the AST from the parse tree. */
	ParseTreeVisitorImpl * parseTreeVisitor = new ParseTreeVisitorImpl(this);
	this->ast = parseTreeVisitor->visitProgram(dynamic_cast<AntlrGrammarGenerated::TParser::ProgramContext *>(this->parseTree));
	std::cout << std::endl << "Printing the AST..." << std::endl;
	std::cout << std::endl << this->ast->printTreeString() << std::endl << std::endl;
}

/** Define a function to perform the semantic analysis, and decorate the symbol table and AST. */
void Compiler::performSemanticAnalysis() {
	this->llvmHandler = new LLVMHandler(this);
	std::cout << "Walking AST to perform remaining semantic analysis..." << std::endl;
	AstVisitorImpl * astVisitor = new AstVisitorImpl(this);
	astVisitor->visitProgram(this->ast);
	// Print the contents of each scoped symbol table, if debugging is enabled. 
	if (this->debuggingOn) {
		for (auto const & curScope : this->symbolTableManager->scopeVector) {
			curScope->print();
			curScope->scopedSymbolTable->printSymbolTable();
		}
	}
	// Print all the generated LLVM IR code.
	this->llvmHandler->printAll();
}



