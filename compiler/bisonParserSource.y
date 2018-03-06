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
%token <cptr> ID INT_KEYWORD VOID_KEYWORD IF_KEYWORD ELSE_KEYWORD WHILE_KEYWORD RETURN_KEYWORD
%type <cptr> declarationList declaration varDeclaration typeSpecifier
%type <cptr> funDeclaration params paramList param compoundStmt
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
varDeclaration		: typeSpecifier ID ';'									{ printf("declaration: Result is %s\n", $1); }
					| typeSpecifier ID '[' NUM ']' ';'									{ printf("declaration: Result is %s\n", $1); }
					;
typeSpecifier		: INT_KEYWORD | VOID_KEYWORD 									{ printf("typeSpecifier: Result is %s\n", $1); }
;
funDeclaration 		: typeSpecifier ID '(' params ')' compoundStmt 									{ printf("funDeclaration: Result is %s\n", $1); }
;
params 				: paramList | VOID_KEYWORD 									{ printf("params: Result is %s\n", $1); }
;
paramList			: paramList ',' param 									{ printf("paramList: Result is %s\n", $1); }
					| param									{ printf("paramList: Result is %s\n", $1); }
					;
param				: typeSpecifier ID 									{ printf("param: Result is %s\n", $1); }
					| typeSpecifier ID '[' ']'									{ printf("param: Result is %s\n", $1); }
					;
compoundStmt		: '{' localDeclaration statementList '}' 									{ printf("compoundStmt: Result is %s\n", $2); }
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
expressionStmt		: expression ';' 									{ printf("expressionStmt: Result is %s\n", $1); }
					| ';'									{ printf("expressionStmt: Result is ;\n"); }
					;
selectionStmt 		: IF_KEYWORD '(' expression ')' statement 									{ printf("selectionStmt: Result is %s\n", $1); }
					| IF_KEYWORD '(' expression ')' statement ELSE_KEYWORD statement									{ printf("selectionStmt: Result is %s\n", $1); }
					;
iterationStmt 		: WHILE_KEYWORD '(' expression ')' statement 									{ printf("iterationStmt: Result is %s\n", $1); }
;
returnStmt 			: RETURN_KEYWORD ';' 									{ printf("returnStmt: Result is %s\n", $1); }
					| RETURN_KEYWORD expression ';'									{ printf("returnStmt: Result is %s\n", $1); }
					;
expression			: var '=' expression 									{ printf("expression: Result is %s\n", $1); }
					| simpleExpression									{ printf("expression: Result is %s\n", $1); }
					;
var 				: ID | ID '[' expression ']' 									{ printf("var: Result is %s\n", $1); }
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
factor 				: '(' expression ')' 									{ printf("sfactorelectionStmt: Result is %s\n", $2); }
					| var 									{ printf("factor: Result is %s\n", $1); }
					| call 									{ printf("factor: Result is %s\n", $1); }
					| NUM									{ printf("factor: Result is %s\n", $1); }
					;
call 				: ID '(' args ')' 									{ printf("call: Result is %s\n", $3); }
;
args 				: argList 									{ printf("args: Result is %s\n", $1); }
					|
					;
argList 			: argList ',' expression 									{ printf("argList: Result is %s\n", $1); }
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





