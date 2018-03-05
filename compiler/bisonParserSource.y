/** Declarations go here. */ 

/** Put any C declarations inside the %{ %} section.
%code {
	#include <stdio.h>
	#define YYSTYPE int
%}

/** Put any Bison declarations after the C declarations. */
%token NUMBER NAME
%left '+' '-'
%left '*' '/'

%%
/** Bison grammar rules go here. */
input	: /* empty production to allow an empty input */
        | input line 
        ;
line    : expr '\n'   		{ printf("Result is %d\n", $1); }
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
        | NUMBER          	{ $$ = $1; printf("Factor: Returning %d\n", $1); }
        ;

%%
/** Additional C code here. */
#include <stdio.h>
#include <ctype.h>
#define YYSTYPE int

/* Required yyerror definition to display error message. */
int yyerror(char * errmsg) {
	printf("ERROR: %s\n", errmsg);
}

/** Define the yylex function to perform the scanning. */


//yywrap()
//{
 //   return(1);
//}

/** Define the main entry point to the program. */
main() {
	printf("Type some input. Enter ? for help.\n");
	yyparse();
}





