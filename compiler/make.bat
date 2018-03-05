:: Batch script for creating the parser using Bison.

:: Call Bison to auto-generate the C code.
win_bison sampleParser.y ^
--output=sampleParser.tab.c ^
--verbose ^
--graph=graph.txt

:: Call GCC to compile the C code into an executable.
gcc -o sampleParser.exe sampleParser.tab.c
