:: Batch script for creating the parser using Bison.

:: Call Flex to auto-generate the C code for the scanner.
win_flex sampleScanner.l

:: Call Bison to auto-generate the C code.
:: -d directive will produce a header file as well.
win_bison -d sampleParser.y ^
--output=sampleParser.tab.c ^
--verbose ^
--graph=graph.txt

:: Call GCC to compile the C code into an executable.
gcc -o sampleParser.exe sampleParser.tab.c lex.yy.c
