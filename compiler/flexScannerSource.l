%{
#include "bisonGeneratedParser.h"
#include <math.h>
inline int yywrap(void) { return 1; };
#define YYERROR_VERBOSE 1

%}

/* flex definitions */
DIGIT [0-9]
LETTER [A-Za-z]

%%
[ \t\n];					/* ignore white space */
{DIGIT}+				{ yylval.intVal = atoi(yytext); return NUM; }
(int)					{ yylval.cptr = yytext; printf("Matched: returning INT\n"); return INT_KEYWORD; }
(void) 					{ yylval.cptr = yytext; return VOID_KEYWORD; }
(if)					{ yylval.cptr = yytext; return IF_KEYWORD; }
(else)					{ yylval.cptr = yytext; return ELSE_KEYWORD; }
(while)					{ yylval.cptr = yytext; return WHILE_KEYWORD; }
(return)				{ yylval.cptr = yytext; return RETURN_KEYWORD; }
"$"						{ return 0; /* end of input. */ }
"="|"+"|"-"|"*"|"/"|"<"|">"|"\n"	{ return yytext[0]; }
"("|")"|"["|"]"|"{"|"}"|","|";" 	{ return yytext[0]; }
"<="|">="|"=="|"!="|"/*"|"*/" 		{ return yytext; }
{LETTER}+				{ yylval.cptr = yytext; return ID; }

%%
/** Define an array of strings to hold the possible keywords. */
/** const char  * keywordList[6];
keywordList[0] = "int";
keywordList[1] = "void";
keywordList[2] = "if";
keywordList[3] = "else";
keywordList[4] = "while";
keywordList[5] = "return";
*/