lexer grammar TLexer;

// These are all supported lexer sections:

// Lexer file header. Appears at the top of h + cpp files. Use e.g. for copyrights.
@lexer::header {/* lexer header section */}

// Appears before any #include in h + cpp files.
@lexer::preinclude {/* lexer precinclude section */}

// Follows directly after the standard #includes in h + cpp files.
@lexer::postinclude {
/* lexer postinclude section */
#ifndef _WIN32
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
}

// Directly preceds the lexer class declaration in the h file (e.g. for additional types etc.).
@lexer::context {/* lexer context section */}

// Appears in the public part of the lexer in the h file.
@lexer::members {/* public lexer declarations section */}

// Appears in the private part of the lexer in the h file.
@lexer::declarations {/* private lexer declarations/members section */}

// Appears in line with the other class member definitions in the cpp file.
@lexer::definitions {/* lexer definitions section */}

channels { CommentsChannel, DirectiveChannel }

tokens {
	
}

/* Define the types of tokens we expect. We must be careful	with the ordering 
	here. ANTLR scanner will match to the first token appearing here, if a token
	matches to multiple. */

// Define these first, otherwise they will fall in the ID or LETTER buckets.
INT_KEYWORD: 'int';
VOID_KEYWORD: 'void';
IF_KEYWORD: 'if';
ELSE_KEYWORD: 'else';
WHILE_KEYWORD: 'while';
RETURN_KEYWORD: 'return';

// Define the full NUM/ID first so these get matched first.
NUM: DIGIT+;
DIGIT: [0-9];
ID: LETTER (LETTER | '0'..'9')*;
LETTER: [a-zA-Z];

// Indicate that after beginning of comment, match anything until the comment terminator is encountered.
COMMENT: '/*' .*? '*/' -> channel(CommentsChannel);

// Define the whitespace token, and push them to a different channel.
WS: [ \t\r\n]+ -> channel(99);

// Define the rest of the tokens.
ASSIGN: '=';
PLUS: '+';
MINUS: '-';
MULTIPLY: '*';
DIVIDE: '/';
LESS_THAN: '<';
GREATER_THAN:  '>';
LEFT_PAREN: '(';
RIGHT_PAREN: ')';
LEFT_BRACES: '{';
RIGHT_BRACES: '}';
LEFT_BRACKET: '[';
RIGHT_BRACKET: ']';
COMMA: ',';
SEMICOLON: ';';
LESS_THAN_OR_EQUAL: '<=';
GREATER_THAN_OR_EQUAL: '>=';
EQUALITY: '==';
NON_EQUALITY: '!=';


//String: '"' .*? '"';



