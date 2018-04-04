/**
 * @author: Kevin Walther
 * @date: 2018
 * @class: ErrorHandler
 */

#include <fstream>
#include <string>
#include <iostream>
#include "ErrorHandler.h"

ErrorHandler::ErrorHandler() {
	return;
}


ErrorHandler::~ErrorHandler() {
	return;
}

/** Define an operator overload for streaming the error code to a string. */
std::ostream & operator<<(std::ostream & os, ErrorHandler::ErrorCodes const errorCode) {
	os << "ERROR ";
	switch (errorCode) {
		case ErrorHandler::ErrorCodes::INVALID_SYNTAX: 
				return os << static_cast<unsigned int>(ErrorHandler::ErrorCodes::INVALID_SYNTAX) << ": Invalid syntax: ";
		case ErrorHandler::ErrorCodes::UNDEFINED_TYPE: 
				return os << static_cast<unsigned int>(ErrorHandler::ErrorCodes::INVALID_SYNTAX) << ": Type is undefined: ";
		case ErrorHandler::ErrorCodes::TYPE_MISMATCH: 
				return os << static_cast<unsigned int>(ErrorHandler::ErrorCodes::INVALID_SYNTAX) << ": Type mismatch: ";
		case ErrorHandler::ErrorCodes::INVALID_TYPE: 
				return os << static_cast<unsigned int>(ErrorHandler::ErrorCodes::INVALID_SYNTAX) << ": Type is not allowed: ";
		case ErrorHandler::ErrorCodes::UNDECL_IDENTIFIER: 
				return os << static_cast<unsigned int>(ErrorHandler::ErrorCodes::INVALID_SYNTAX) << ": Undeclared identifier: ";
		case ErrorHandler::ErrorCodes::TOO_FEW_ARGS: 
				return os << static_cast<unsigned int>(ErrorHandler::ErrorCodes::INVALID_SYNTAX) << ": Too few arguments for function: ";
		case ErrorHandler::ErrorCodes::TOO_MANY_ARGS: 
				return os << static_cast<unsigned int>(ErrorHandler::ErrorCodes::INVALID_SYNTAX) << ": Too many arguments for function: ";
		case ErrorHandler::ErrorCodes::MISSING_RETURN_VAL: 
				return os << static_cast<unsigned int>(ErrorHandler::ErrorCodes::INVALID_SYNTAX) << ": Return value expected for function: ";
		case ErrorHandler::ErrorCodes::NO_RETURN_VAL: 
				return os << static_cast<unsigned int>(ErrorHandler::ErrorCodes::INVALID_SYNTAX) << ": No return value expected for function: ";
	};
	return os;
}

