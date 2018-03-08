:: Batch script for creating the parser using Bison.

:: Call Flex to auto-generate the C code for the scanner.
win_flex --outfile=flexGeneratedScanner.c flexScannerSource.l

:: Call Bison to auto-generate the C code.
:: -d directive will produce a header file as well.
win_bison -d bisonParserSource.y ^
--output=bisonGeneratedParser.c ^
--verbose ^
--graph=parseGraph.dot

:: Call GCC to compile the C code into an executable.
:: Put gcc option '-w' to hide all warnings.
gcc -o flexBisonScannerParser.exe bisonGeneratedParser.c flexGeneratedScanner.c -w

:: Run dot.exe on the generated Bison digraph to generate visualization.
dot -T pdf parseGraph.dot -o parseGraph.pdf