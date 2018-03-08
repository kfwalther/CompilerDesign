/** Declarations go here. */ 

/** Put any C declarations inside the %{ %} section. */
/** TODO: Figure out why these declarations aren't getting to the code section below. */
/** %code {
	#include <stdio.h>
	#define YYSTYPE int
%} */

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
%token ASSIGN PLUS MINUS MULTIPLY DIVIDE LESS_THAN GREATER_THAN
%token LESS_THAN_OR_EQUAL GREATER_THAN_OR_EQUAL EQUALITY NON_EQUALITY COMMENT_START COMMENT_END

/** %type <cptr> declarationList declaration varDeclaration typeSpecifier
%type <cptr> funDeclaration params compoundStmt
%type <cptr> localDeclaration statementList statement expressionStmt selectionStmt
%type <cptr> iterationStmt returnStmt expression var simpleExpression
%type <cptr> relop additiveExpression addop term mulop factor call
%type <cptr> args argList */
%right '='
%left '+' '-'
%left '*' '/'

%%
/** Bison grammar rules go here. */
program				: declarationList							{ printf("Program: \n"); }
					;
declarationList		: declarationList declaration 					{ printf("declarationList: declarationList declaration\n"); }
					| declaration									{ printf("declarationList: declaration\n"); }
					;
declaration			: varDeclaration | funDeclaration 				{ printf("declaration: varDeclaration/funDeclarations\n"); }
					;
varDeclaration		: typeSpecifier ID SEMICOLON					{ printf("varDeclaration: typeSpecifier ID SEMICOLON\n"); }
					| typeSpecifier ID LEFT_BRACKET NUM RIGHT_BRACKET SEMICOLON		{ printf("varDeclaration: typeSpecifier ID LEFT_BRACKET NUM RIGHT_BRACKET SEMICOLON\n"); }
					;
typeSpecifier		: INT_KEYWORD | VOID_KEYWORD 					{ printf("typeSpecifier: Result is INT/VOID\n"); }
;
funDeclaration 		: typeSpecifier ID LEFT_PAREN params RIGHT_PAREN compoundStmt 	{ printf("funDeclaration: \n"); }
;
params 				: paramList | VOID_KEYWORD 					{ printf("params: \n"); }
;
paramList			: paramList COMMA param 					{ printf("paramListN: paramList COMMA param\n"); }
					| param									{ printf("paramList1: param\n"); }
					;
param				: typeSpecifier ID 									{ printf("param: typeSpecifier ID\n"); }
					| typeSpecifier ID LEFT_BRACKET RIGHT_BRACKET		{ printf("param: typeSpecifier ID LEFT_BRACKET RIGHT_BRACKET\n"); }
					;
compoundStmt		: LEFT_BRACES localDeclaration statementList RIGHT_BRACES 		{ printf("compoundStmt:\n"); }
;
localDeclaration 	: localDeclaration varDeclaration 					{ printf("localDeclaration: localDeclaration varDeclaration\n"); }
					| 													{ printf("localDeclaration: [empty]\n"); }
					;
statementList 		: statementList statement 							{ printf("statementList: statementList statement\n"); }
					| 													{ printf("localDeclaration: [empty]\n"); }
					;
statement 			: expressionStmt 									{ printf("statement: expressionStmt\n"); }
					| compoundStmt 									{ printf("statement: compoundStmt\n"); }
					| selectionStmt 									{ printf("statement: selectionStmt\n"); }
					| iterationStmt 									{ printf("statement: iterationStmt\n"); }
					| returnStmt									{ printf("statement: returnStmt\n"); }
					;
expressionStmt		: expression SEMICOLON 									{ printf("expressionStmt: expression ;\n"); }
					| SEMICOLON									{ printf("expressionStmt: ;\n"); }
					;
selectionStmt 		: IF_KEYWORD LEFT_PAREN expression RIGHT_PAREN statement 		{ printf("selectionStmt: if ( exp ) { }\n"); }
					| IF_KEYWORD LEFT_PAREN expression RIGHT_PAREN statement ELSE_KEYWORD statement		{ printf("selectionStmt: if ( exp ) { } else {}\n"); }
					;
iterationStmt 		: WHILE_KEYWORD LEFT_PAREN expression RIGHT_PAREN statement 			{ printf("iterationStmt:\n"); }
;
returnStmt 			: RETURN_KEYWORD SEMICOLON 					{ printf("returnStmt: return ;\n"); }
					| RETURN_KEYWORD expression SEMICOLON		{ printf("returnStmt: return ___; \n"); }
					;
expression			: var ASSIGN expression 							{ printf("expression: var ASSIGN expression \n"); }
					| simpleExpression									{ printf("expression: simpleExpression\n"); }
					;
var 				: ID | ID LEFT_BRACKET expression RIGHT_BRACKET 		{printf("var: \n"); }
;
simpleExpression 	: additiveExpression relop additiveExpression 	{ printf("simpleExpression: additiveExpression relop additiveExpression\n"); }
					| additiveExpression							{ printf("simpleExpression: additiveExpression\n"); }
					;
relop 				: LESS_THAN_OR_EQUAL | GREATER_THAN_OR_EQUAL | EQUALITY | NON_EQUALITY | LESS_THAN | GREATER_THAN ;
additiveExpression 	: additiveExpression addop term | term ;
addop 				: PLUS | MINUS ;
term 				: term mulop factor 	{ printf("term: \n"); }
| factor 									{ printf("term: \n"); }
;
mulop 				: MULTIPLY | DIVIDE ;
factor 				: LEFT_PAREN expression RIGHT_PAREN 	{ printf("factor: ( exp )\n"); }
					| var 									{ printf("factor: var\n"); }
					| call 									{ printf("factor: call\n"); }
					| NUM									{ printf("factor: NUM\n"); }
					;
call 				: ID LEFT_PAREN args RIGHT_PAREN 									{ printf("call:\n"); }
;
args 				:  									
					| argList							{ printf("args: \n"); }
					;
argList 			: argList COMMA expression 									{ printf("argList: argList COMMA expression \n"); }
					| expression									{ printf("argList: expression\n"); }
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





