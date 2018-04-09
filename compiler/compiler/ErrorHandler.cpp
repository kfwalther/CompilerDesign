/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: ErrorHandler
 */

#include <fstream>
#include <string>
#include <iostream>
#include "ErrorHandler.h"


ErrorHandler::ErrorHandler(std::string const & inputFile) : inputFile(inputFile) {
	return;
}

ErrorHandler::~ErrorHandler() {
	return;
}

/** Define an operator overload for streaming the error code to a string. */
std::ostream & operator<<(std::ostream & os, ErrorHandler::ErrorCodes const errorCode) {
	os << "ERROR ";
	switch (errorCode) {
		case ErrorHandler::ErrorCodes::COMPILER_ERROR:
				return os << static_cast<unsigned int>(errorCode) << ": Internal compiler error: ";
		case ErrorHandler::ErrorCodes::INVALID_SYNTAX: 
				return os << static_cast<unsigned int>(errorCode) << ": Invalid syntax: ";
		case ErrorHandler::ErrorCodes::UNDEFINED_TYPE: 
				return os << static_cast<unsigned int>(errorCode) << ": Type is undefined: ";
		case ErrorHandler::ErrorCodes::TYPE_MISMATCH: 
				return os << static_cast<unsigned int>(errorCode) << ": Type mismatch: ";
		case ErrorHandler::ErrorCodes::INVALID_TYPE: 
				return os << static_cast<unsigned int>(errorCode) << ": Type is not allowed: ";
		case ErrorHandler::ErrorCodes::UNDECL_IDENTIFIER: 
				return os << static_cast<unsigned int>(errorCode) << ": Undeclared identifier: ";
		case ErrorHandler::ErrorCodes::TOO_FEW_ARGS: 
				return os << static_cast<unsigned int>(errorCode) << ": Too few arguments for function: ";
		case ErrorHandler::ErrorCodes::TOO_MANY_ARGS: 
				return os << static_cast<unsigned int>(errorCode) << ": Too many arguments for function: ";
		case ErrorHandler::ErrorCodes::MISSING_RETURN_VAL: 
				return os << static_cast<unsigned int>(errorCode) << ": Return value expected for function: ";
		case ErrorHandler::ErrorCodes::NO_RETURN_VAL:
				return os << static_cast<unsigned int>(errorCode) << ": No return value expected for function: ";
		case ErrorHandler::ErrorCodes::NO_MATCHING_SIGNATURE:
				return os << static_cast<unsigned int>(errorCode) << ": No matching function signature found for function: ";
	};
	return os;
}

/** Define a function to print out an error message, given some context of the error. */
void ErrorHandler::printError(ErrorHandler::ErrorCodes const & errorCode, size_t const & errorLine, std::string const & errorMessage) {
	// Get the line in the file where the error occurred.
	std::string errorLocation = this->inputFile + "(" + std::to_string(errorLine) + "): ";
	std::cerr << errorLocation << errorCode << std::endl << errorLocation << errorMessage << std::endl << std::endl;
}

