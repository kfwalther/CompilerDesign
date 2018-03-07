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
[ \t]					{ printf("Ignoring whitespace...\n"); }
\n						{ printf("Ignoreing newlines...\n"); }
{DIGIT}+				{ yylval.intVal = atoi(yytext); return NUM; }
(int)					{ printf("Matched: returning INT\n"); return INT_KEYWORD; }
(void) 					{ return VOID_KEYWORD; }
(if)					{ return IF_KEYWORD; }
(else)					{ return ELSE_KEYWORD; }
(while)					{ return WHILE_KEYWORD; }
(return)				{ return RETURN_KEYWORD; }
"$"						{ return 0; /* end of input. */ }
"="|"+"|"-"|"*"|"/"|"<"|">"|"\n"	{ return yytext[0]; }
"("						{ return LEFT_PAREN; }
")"						{ return RIGHT_PAREN; }
"["						{ return LEFT_BRACKET; }
"]"						{ return RIGHT_BRACKET; }
"{"						{ return LEFT_BRACES; }
"}"						{ return RIGHT_BRACES; }
","						{ return COMMA; }
";"						{ return SEMICOLON; }
"<="|">="|"=="|"!="|"/*"|"*/" 		{ return yytext; }
{LETTER}+				{ yylval.cptr = yytext; printf("Matched: returning %s\n", yylval.cptr); return ID; }

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