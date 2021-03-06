'''

@author: Kevin Walther
@class: Compiler Design 605.415 Spring 2018
@assignment: Homework 2
@brief: This Python program implements a spreadsheet scanner parser.

'''

import os
import sys
import ScannerParser
import Cell

# Process the input file line by line.
def processInputFile(inputFilePath):
    with open(inputFilePath, 'r') as fileId:
        scannerParser = ScannerParser.ScannerParser()
        for line in fileId:
            if line[0] == '#':
                continue
            else:
                try:
                    scannerParser.processLine(line)
                except ValueError as err:
                    print('WARNING: ' + err.args[0])
                    continue
        scannerParser.spreadsheet.printSpreadsheet()
#         scannerParser.spreadsheet.printCellInfo()
    return scannerParser

# Run the Scanner Parser.
def runScannerParser(inputFile, outputFile):
    inputFilePath = os.path.join(os.path.dirname(os.path.abspath(__file__)), inputFile)
    scannerParser = processInputFile(inputFilePath)

# Begin code execution here.
if __name__ == "__main__":
    runScannerParser('HW5-Input.txt', 'output.txt')
    










