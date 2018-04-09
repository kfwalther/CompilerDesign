#ifndef ERRORCODES_H
#define ERRORCODES_H

/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: ErrorHandler
 */


struct ErrorHandler {

	/** Alias some commonly used types for convenience. */

	/** Define the constructors. */
	ErrorHandler(std::string const & inputFile);
	~ErrorHandler();
	
	/** Define the C-Minus language native types. */
	enum class ErrorCodes : unsigned int { 
		COMPILER_ERROR = 0,
		INVALID_SYNTAX,
		UNDEFINED_TYPE,
		TYPE_MISMATCH,
		INVALID_TYPE,
		UNDECL_IDENTIFIER,
		TOO_FEW_ARGS,
		TOO_MANY_ARGS,
		MISSING_RETURN_VAL,
		NO_RETURN_VAL,
		NO_MATCHING_SIGNATURE,
		LVAL_MISUSE
	};
	
	/** Define the input file we will be compiling. */
	std::string inputFile;

	/** Define a function to print out an error message, given some context of the error. */
	void printError(ErrorHandler::ErrorCodes const & errorCode, size_t const & errorLine, std::string const & errorMessage);

	/** Define an operator overload for streaming the error code to a string. */
	friend std::ostream & operator<<(std::ostream & os, ErrorHandler::ErrorCodes const errorCode);
};


#endif /* ERRORCODES_H */
