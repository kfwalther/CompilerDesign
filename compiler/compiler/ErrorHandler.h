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
	ErrorHandler();
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
		NO_RETURN_VAL
	};
	
	/** Define an operator overload for streaming the error code to a string. */
	friend std::ostream & operator<<(std::ostream & os, ErrorHandler::ErrorCodes const errorCode);
};


#endif /* ERRORCODES_H */
