'''

@author: Kevin Walther
@class: Compiler Design 605.415 Spring 2018
@assignment: Homework 2
@brief: This Python program implements a spreadsheet scanner parser.

'''

import os
import sys
import Scanner


# Process the input file line by line.
def processInputFile(inputFilePath):
    with open(inputFilePath, 'r') as fileId:
        scanner = Scanner.Scanner()
        for line in fileId:
            if line[0] == '#':
                continue
            else:
                try:
                    scanner.processLine(line)
                except ValueError as err:
                    print('WARNING: ' + err.args[0])
                    continue
        scanner.printTokens()
    return scanner

# Run the Scanner Parser.
def runScannerParser(inputFile, outputFile):
    inputFilePath = os.path.join(os.path.dirname(os.path.abspath(__file__)), inputFile)
#     outputFilePath = os.path.join(os.path.dirname(os.path.abspath(__file__)), outputFile)
    scanner = processInputFile(inputFilePath)
    # Use these functions to test the Scanner.getCell/clearCell methods.
#     print(str(scanner.getCell('A0')))
#     print(str(scanner.getCell('C5')))
#     scanner.clearCell('C5')
#     scanner.printTokens()

# Begin code execution here.
if __name__ == "__main__":
    runScannerParser('HW3-Input.txt', 'output.txt')
    










