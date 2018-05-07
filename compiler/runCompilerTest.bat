
:: Set the directory where the project is located.
set WORKING_DIR=C:\Users\walther\CompilerDesign\compiler
set Path=%WORKING_DIR%\VS15W64\lib\Debug;%Path%

cd %WORKING_DIR%\VS15W64\compiler\Debug

:: Loop through each test input file and generate an output file.
:: Redirect stdout and stderr to the same file.
for %%a in (
	TestCase1.txt
	TestCase2.txt
	TestCase3.txt
	TestCase4.txt
	TestCase5.txt
	TestCase6.txt
	TestCase7.txt
	TestCase8.txt
	TestCase9.txt
	TestCase10.txt
	TestCase11.txt
	TestCase12.txt
	C-Input-1.txt
	C-Input-2.txt
	C-Input-3.txt
) do ANTLR4GrammarTest.exe %WORKING_DIR%\tests\%%a > %WORKING_DIR%\tests\output\Output-%%a 2>&1


PAUSE



