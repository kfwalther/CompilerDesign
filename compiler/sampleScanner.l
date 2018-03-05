%{
#include "sampleParser.tab.h"
//#include "parser.h"
#include <math.h>
inline int yywrap(void) { return 1; };
%}

/* flex definitions */
DIGIT [0-9]
/** LETTER [A-Za-z] */

%%
[ \t];					/* ignore white space */

{DIGIT}+				{ yylval = atoi(yytext); return NUMBER; }
"$"						{ return 0; /* end of input. */ }
"\n"|"="|"+"|"-"|"*"|"/"|"("|")" { return yytext[0]; }

%%
/* main method calls yylex to tokenize input */
/**int main() { 
	printf("Type something: ");
	while( yylex() > 0 ) { }; 
} */
