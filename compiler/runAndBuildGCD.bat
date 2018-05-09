:: Script to run the C-Input-1.txt input file through C-Minus compiler,
:: generate LLVM, compile the LLVM to assembly on target machine, then
:: compile the assembly to C++ code.

set WORKING_DIR=D:\workspace\CompilerDesign\compiler
cd %WORKING_DIR%

:: Call the C-Minus compiler executable to generate LLVM.
%WORKING_DIR%\VS15W64\compiler\Debug\ANTLR4GrammarTest.exe %WORKING_DIR%\tests\C-Input-1.txt > ^
%WORKING_DIR%\tests\output\Output-gcd.txt 2> %WORKING_DIR%\tests\output\gcd.bc

cd %WORKING_DIR%\tests\output
:: Compile the LLVM into assembly source code.
llc gcd.bc

:: Compile the assembly and a C file into an executable.
gcc ..\..\compiler\inputOutput.c gcd.s -o gcd.exe

