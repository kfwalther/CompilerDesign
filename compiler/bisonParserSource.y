/** Declarations go here. */ 

/** Put any C declarations inside the %{ %} section.
%code {
	#include <stdio.h>
	#define YYSTYPE int
%}

/** Put any Bison declarations after the C declarations. */
/** Define the attributes to add to yylval. */
%union {
int intVal; 	// Return an int.
char * cptr;	// Return a string.
}

/** Define the type of each grammar rule, as well as precedences. */
%token <intVal> NUM 
%token ID 
%token INT_KEYWORD VOID_KEYWORD IF_KEYWORD ELSE_KEYWORD WHILE_KEYWORD RETURN_KEYWORD
%token LEFT_PAREN RIGHT_PAREN LEFT_BRACKET RIGHT_BRACKET LEFT_BRACES RIGHT_BRACES COMMA SEMICOLON
%type <cptr> declarationList declaration varDeclaration typeSpecifier
%type <cptr> funDeclaration 
%type params 
%type <cptr> compoundStmt
%type <cptr> localDeclaration statementList statement expressionStmt selectionStmt
%type <cptr> iterationStmt returnStmt expression var simpleExpression
%type <cptr> relop additiveExpression addop term mulop factor call
%type <cptr> args argList
%right '='
%left '+' '-'
%left '*' '/'

%%
/** Bison grammar rules go here. */
program				: declarationList											{ printf("Program: Result is %s\n", $1); }
					;
declarationList		: declarationList declaration 									{ printf("declarationList: Result is %s\n", $1); }
					| declaration									{ printf("declarationList: Result is %s\n", $1); }
					;
declaration			: varDeclaration | funDeclaration 									{ printf("declaration: Result is %s\n", $1); }
					;
varDeclaration		: typeSpecifier ID SEMICOLON									{ printf("declaration: Result is %s\n", $1); }
					| typeSpecifier ID LEFT_BRACKET NUM RIGHT_BRACKET SEMICOLON									{ printf("declaration: Result is %s\n", $1); }
					;
typeSpecifier		: INT_KEYWORD | VOID_KEYWORD 									{ printf("typeSpecifier: Result is INT/VOID\n"); }
;
funDeclaration 		: typeSpecifier ID LEFT_PAREN params RIGHT_PAREN compoundStmt 									{ printf("funDeclaration: Result is\n"); }
;
params 				: paramList | VOID_KEYWORD 									{ printf("params: Result is a list\n"); }
;
paramList			: paramList COMMA param 									{ printf("paramListN: Result is\n"); }
					| param									{ printf("paramList1: Result is\n"); }
					;
param				: typeSpecifier ID 									{ printf("param: Result is\n"); }
					| typeSpecifier ID LEFT_BRACKET RIGHT_BRACKET									{ printf("param: Result is \n"); }
					;
compoundStmt		: LEFT_BRACES localDeclaration statementList RIGHT_BRACES 									{ printf("compoundStmt: Result is %s\n", $2); }
;
localDeclaration 	: localDeclaration varDeclaration 									{ printf("localDeclaration: Result is %s\n", $1); }
					| 
					;
statementList 		: statementList statement 									{ printf("statementList: Result is %s\n", $1); }
					|
					;
statement 			: expressionStmt 									{ printf("statement: Result is %s\n", $1); }
					| compoundStmt 									{ printf("statement: Result is %s\n", $1); }
					| selectionStmt 									{ printf("statement: Result is %s\n", $1); }
					| iterationStmt 									{ printf("statement: Result is %s\n", $1); }
					| returnStmt									{ printf("statement: Result is %s\n", $1); }
					;
expressionStmt		: expression SEMICOLON 									{ printf("expressionStmt: Result is %s\n", $1); }
					| SEMICOLON									{ printf("expressionStmt: Result is ;\n"); }
					;
selectionStmt 		: IF_KEYWORD LEFT_PAREN expression RIGHT_PAREN statement 									{ printf("selectionStmt: \n"); }
					| IF_KEYWORD LEFT_PAREN expression RIGHT_PAREN statement ELSE_KEYWORD statement									{ printf("selectionStmt: \n"); }
					;
iterationStmt 		: WHILE_KEYWORD LEFT_PAREN expression RIGHT_PAREN statement 									{ printf("iterationStmt:\n"); }
;
returnStmt 			: RETURN_KEYWORD SEMICOLON 									{ printf("returnStmt: Result is RETURN\n"); }
					| RETURN_KEYWORD expression SEMICOLON									{ printf("returnStmt: Result is RETURN\n"); }
					;
expression			: var '=' expression 									{ printf("expression: Result is %s\n", $1); }
					| simpleExpression									{ printf("expression: Result is %s\n", $1); }
					;
var 				: ID | ID LEFT_BRACKET expression RIGHT_BRACKET 									{printf("var: Result is\n"); }
;
simpleExpression 	: additiveExpression relop additiveExpression 									{ printf("simpleExpression: Result is %s\n", $1); }
					| additiveExpression									{ printf("simpleExpression: Result is %s\n", $1); }
					;
relop 				: "<=" | ">=" | "==" | "!=" | "<" | ">" ;
additiveExpression 	: additiveExpression addop term | term ;
addop 				: '+' | '-' ;
term 				: term mulop factor 									{ printf("term: Result is %s\n", $1); }
| factor 									{ printf("term: Result is %s\n", $1); }
;
mulop 				: '*' | '/' ;
factor 				: LEFT_PAREN expression RIGHT_PAREN 									{ printf("factor:\n"); }
					| var 									{ printf("factor: Result is %s\n", $1); }
					| call 									{ printf("factor: Result is %s\n", $1); }
					| NUM									{ printf("factor: Result is %s\n", $1); }
					;
call 				: ID LEFT_PAREN args RIGHT_PAREN 									{ printf("call:\n"); }
;
args 				:  									
					| argList							{ printf("args: Result is %s\n", $1); }
					;
argList 			: argList COMMA expression 									{ printf("argList: Result is %s\n", $1); }
					| expression									{ printf("argList: Result is %s\n", $1); }
					;

/**
input	:
        | input line 
        ;
line    : expr '\n'   		{ printf("Result is %d\n", $1); }
		| ID '\n'			{ printf("Result is %s\n", $1); }
		;
expr    : expr '+' term   	{ $$ = $1 + $3;  printf("expr: Returning %d\n", $$); }
        | expr '-' term   	{ $$ = $1 - $3; }
        | term            	{ $$ = $1; }
        ;
term    : term '*' factor 	{ $$ = $1 * $3; }
        | term '/' factor 	{ 
								if ($3 == 0) {
									yyerror("Divide by zero!");
								} else {
									$$ = $1 / $3; 
								}
							}
        | factor          	{ $$ = $1; }
        ;
factor  : '(' expr ')'    	{ $$ = $2; }
    	| '-' factor      	{ $$ = -$2; }
        | NUM          		{ $$ = $1; printf("Factor: Returning %d\n", $1); }
        ;
*/
%%
/** Additional C code here. */
#include <stdio.h>
#include <ctype.h>
#define YYSTYPE int

/* Required yyerror definition to display error message. */
int yyerror(char * errmsg) {
	printf("ERROR: %s\n", errmsg);
}


/** Define the main entry point to the program. */
main() {
	printf("Type some input. Enter ? for help.\n");
	yyparse();
}





