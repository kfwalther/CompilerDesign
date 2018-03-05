/** Declarations go here. */ 

/** Put any C declarations inside the %{ %} section.
%code {
	#include <stdio.h>
	#define YYSTYPE int
%}

/** Put any Bison declarations after the C declarations. */
%token NUMBER
%left '+' '-'
%left '*' '/'

%%
/** Bison grammar rules go here. */
 input   : /* empty production to allow an empty input */
        | input line 
        ;
line    : expr '\n'   		{ printf("Result is %d\n", $1); }
		;
expr    : expr '+' term   { $$ = $1 + $3; }
        | expr '-' term   { $$ = $1 - $3; }
        | term            { $$ = $1; }
        ;
term    : term '*' factor { $$ = $1 * $3; }
        | term '/' factor { $$ = $1 / $3; }
        | factor          { $$ = $1; }
        ;
factor  : '(' expr ')'    { $$ = $2; }
        | NUMBER          { $$ = $1; }
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
int yylex() {
	int c = getchar();		/* read from stdin */
	if (c < 0) {
		printf("Returning EOF\n");
		return 0;		/* end of input */
	}
	/** Check for operators. */
	if (c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')') {
		printf("Returning operator\n");
		return c;
	}
	if (c == '\n') {
		printf("Returning newline\n");
		return c;
	}
	/* For character tokens, TYPE = character itself. Read 
		number and store as yylval */
	if (isdigit(c)) {
		yylval = c - '0';	/* yylval is a global var */
		while (isdigit(c = getchar())) {
			yylval = 10 * yylval + (c - '0');
		}
		if (c >= 0) ungetc(c, stdin);
		printf("Returning int: %d\n", yylval);
		return NUMBER;	/* token type is NUMBER  */
	}
}

//yywrap()
//{
 //   return(1);
//}

/** Define the main entry point to the program. */
main() {
	printf("Type some input. Enter ? for help.\n");
	yyparse();
}





