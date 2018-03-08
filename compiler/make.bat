:: Batch script for creating the parser using Bison.

:: Call Bison to auto-generate the C code.
:: -d directive will produce a header file as well.
win_bison -d bisonParserSource.ypp ^
--output=bisonGeneratedParser.cpp ^
--verbose ^
--graph=parseGraph.dot

:: Call Flex to auto-generate the C code for the scanner.
win_flex --c++ --outfile=flexGeneratedScanner.cpp flexScannerSource.l
::win_flex flexScannerSource.l

:: Call GCC to compile the C code into an executable.
:: Put gcc option '-w' to hide all warnings.
:: Put gcc option '-Wall -g' to get all warnings.
g++ -c bisonGeneratedParser.cpp -o bisonGeneratedParser.o -Wall -isystem "C:\Users\walther\win_flex_bison-2.5.1" 
g++ -c flexGeneratedScanner.cpp -o flexGeneratedScanner.o -Wall -isystem "C:\Users\walther\win_flex_bison-2.5.1" 
g++ flexGeneratedScanner.o bisonGeneratedParser.o -o flexBisonScannerParser.exe -w 

:: Run dot.exe on the generated Bison digraph to generate visualization.
::dot -T pdf parseGraph.dot -o parseGraph.pdf