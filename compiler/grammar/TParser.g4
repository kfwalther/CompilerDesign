parser grammar TParser;

options {
	tokenVocab = TLexer;
}

// These are all supported parser sections:

// Parser file header. Appears at the top in all parser related files. Use e.g. for copyrights.
@parser::header {/* parser/listener/visitor header section */}

// Appears before any #include in h + cpp files.
@parser::preinclude {/* parser precinclude section */}

// Follows directly after the standard #includes in h + cpp files.
@parser::postinclude {
/* parser postinclude section */
#include "SymbolTable.h"
#include "AstNode.h"

#ifndef _WIN32
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
}

// Directly preceeds the parser class declaration in the h file (e.g. for additional types etc.).
@parser::context {
/* parser context section */
}

// Appears in the public part of the parser in the h file.
// The function bodies could also appear in the definitions section, but I want to maximize
// Java compatibility, so we can also create a Java parser from this grammar.
// Still, some tweaking is necessary after the Java file generation (e.g. bool -> boolean).
@parser::members {
/* public parser declarations/members section */

/** Define a SymbolTable object to store all of the tokens we encounter. */
std::shared_ptr<SymbolTable> symbolTable;
void initializeSymbolTable() { this->symbolTable = std::make_shared<SymbolTable>(); }
std::shared_ptr<SymbolTable> const getSymbolTable() { return this->symbolTable; }
void setSymbolTable(std::shared_ptr<SymbolTable> symbolTable) { this->symbolTable = symbolTable; }

/** Define an AstNode object to hold the root of the AST as it is constructed. */
std::shared_ptr<AstNode> abstractSyntaxTree;

bool myAction() { return true; }
bool doesItBlend() { return true; }
void cleanUp() {}
void doInit() {}
void doAfter() {}
}

// Appears in the private part of the parser in the h file.
@parser::declarations {
/* private parser declarations/members section */
}

// Appears in line with the other class member definitions in the cpp file.
@parser::definitions {/* parser definitions section */}

// Additionally there are similar sections for (base)listener and (base)visitor files.
@parser::listenerpreinclude {/* listener preinclude section */}
@parser::listenerpostinclude {/* listener postinclude section */}
@parser::listenerdeclarations {/* listener public declarations/members section */}
@parser::listenermembers {/* listener private declarations/members section */}
@parser::listenerdefinitions {/* listener definitions section */}

@parser::baselistenerpreinclude {/* base listener preinclude section */}
@parser::baselistenerpostinclude {/* base listener postinclude section */}
@parser::baselistenerdeclarations {/* base listener public declarations/members section */}
@parser::baselistenermembers {/* base listener private declarations/members section */}
@parser::baselistenerdefinitions {/* base listener definitions section */}

@parser::visitorpreinclude {/* visitor preinclude section */}
@parser::visitorpostinclude {/* visitor postinclude section */}
@parser::visitordeclarations {/* visitor public declarations/members section */}
@parser::visitormembers {/* visitor private declarations/members section */}
@parser::visitordefinitions {/* visitor definitions section */}

@parser::basevisitorpreinclude {/* base visitor preinclude section */}
@parser::basevisitorpostinclude {/* base visitor postinclude section */}
@parser::basevisitordeclarations {/* base visitor public declarations/members section */}
@parser::basevisitormembers {/* base visitor private declarations/members section */}
@parser::basevisitordefinitions {/* base visitor definitions section */}

// Actual grammar start.
program				: declarationList ;
declarationList		: declarationList declaration | declaration ;
declaration			: varDeclaration | funDeclaration ;
varDeclaration		: typeSpecifier ID SEMICOLON
					| typeSpecifier ID LEFT_BRACKET NUM RIGHT_BRACKET SEMICOLON
					;
typeSpecifier		: INT_KEYWORD | VOID_KEYWORD ;
funDeclaration 		: typeSpecifier ID LEFT_PAREN params RIGHT_PAREN compoundStmt ;
params 				: paramList | VOID_KEYWORD ;
paramList			: paramList COMMA param | param	;
param				: typeSpecifier ID
					| typeSpecifier ID LEFT_BRACKET RIGHT_BRACKET
					;
compoundStmt		: LEFT_BRACES localDeclaration statementList RIGHT_BRACES ;
localDeclaration 	: localDeclaration varDeclaration
					| 
					;
statementList 		: statementList statement 
					| 
					;
statement 			: expressionStmt
					| compoundStmt
					| selectionStmt
					| iterationStmt 
					| returnStmt
					;
expressionStmt		: expression SEMICOLON | SEMICOLON ;
selectionStmt 		: IF_KEYWORD LEFT_PAREN expression RIGHT_PAREN statement
					| IF_KEYWORD LEFT_PAREN expression RIGHT_PAREN statement ELSE_KEYWORD statement
					;
iterationStmt 		: WHILE_KEYWORD LEFT_PAREN expression RIGHT_PAREN statement ;
returnStmt 			: RETURN_KEYWORD SEMICOLON 
					| RETURN_KEYWORD expression SEMICOLON
					;
expression			: <assoc = right> var ASSIGN expression | simpleExpression ;
var 				: ID | ID LEFT_BRACKET expression RIGHT_BRACKET ;
simpleExpression 	: additiveExpression relop additiveExpression | additiveExpression ;
relop 				: LESS_THAN_OR_EQUAL | GREATER_THAN_OR_EQUAL | EQUALITY | NON_EQUALITY | LESS_THAN | GREATER_THAN ;
additiveExpression 	: <assoc = left> additiveExpression addop term | term ;
addop 				: PLUS | MINUS ;
term 				: <assoc = left> term mulop factor | factor ;
mulop 				: MULTIPLY | DIVIDE ;
factor 				: LEFT_PAREN expression RIGHT_PAREN
					| var
					| call
					| NUM
					;
call 				: ID LEFT_PAREN args RIGHT_PAREN ;
args 				: argList							
					| 
					;
argList 			: argList COMMA expression | expression ;

