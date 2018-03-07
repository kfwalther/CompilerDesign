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
\n						{ printf("Ignoring newlines...\n"); }
{DIGIT}+				{ printf("Matched NUM: %d\n", atoi(yytext)); yylval.intVal = atoi(yytext); return NUM; }
(int)					{ printf("Matched Keyword: int\n"); return INT_KEYWORD; }
(void) 					{ printf("Matched Keyword: void\n"); return VOID_KEYWORD; }
(if)					{ printf("Matched Keyword: if\n"); return IF_KEYWORD; }
(else)					{ printf("Matched Keyword: else\n"); return ELSE_KEYWORD; }
(while)					{ printf("Matched Keyword: while\n"); return WHILE_KEYWORD; }
(return)				{ printf("Matched Keyword: return\n"); return RETURN_KEYWORD; }
"$"						{ return 0; /* end of input. */ }
"="						{ return ASSIGN; }      
"+"						{ return PLUS; }
"-"						{ return MINUS; }
"*"						{ return MULTIPLY; }
"/"						{ return DIVIDE; }
"<"						{ return LESS_THAN; }
">"						{ return GREATER_THAN; }
"("						{ return LEFT_PAREN; }
")"						{ return RIGHT_PAREN; }
"["						{ return LEFT_BRACKET; }
"]"						{ return RIGHT_BRACKET; }
"{"						{ return LEFT_BRACES; }
"}"						{ return RIGHT_BRACES; }
","						{ return COMMA; }
";"						{ return SEMICOLON; }     
"<="					{ return LESS_THAN_OR_EQUAL; }
">="					{ return GREATER_THAN_OR_EQUAL; }
"=="					{ return EQUALITY; }
"!="					{ return NON_EQUALITY; }
"/*"					{ return COMMENT_START; }
"*/" 					{ return COMMENT_END; }
{LETTER}+				{ yylval.cptr = yytext; printf("Matched ID: returning %s\n", yylval.cptr); return ID; }

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