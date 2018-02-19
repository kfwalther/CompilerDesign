README.txt

author: Kevin Walther

My code is separated into three main class files and a Python script to run the 
program. The three main class files define Token, Cell, and Scanner objects. The 
Token objects are used for expressions only at this point, and these tokens can
have types of ID, Number, or Special Character. The Spreadsheet is comprised of 
a Python Dictionary of Cell objects. Each Cell object can be of type Token, Number,
Text, or Empty. The expression Cells that are encountered have a minimal finite 
state machine to parse the expression contents into a list of Token objects. This
list is stored in the appropriate Cell object. The Scanner object holds the entire
Spreadsheet (dictionary of Cell objects) along with some other state-ful variables
used during the parse. Of note is the Scanner's method called getCell. The getCell
method will return the contents of a specified Cell.

This code can be run by calling the runScannerParser.py file. The code was tested on
Windows 7 using Python version 3.6.3

