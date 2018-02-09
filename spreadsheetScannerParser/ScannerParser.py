'''

@author: Kevin Walther
@class: Compiler Design 605.415 Spring 2018
@assignment: Homework 2
@brief: This Python program implements a spreadsheet scanner parser.

'''

import os
import sys



# Process the input file line by line.
def processInputFile(inputFilePath):
  with open(inputFilePath, 'r') as fileId:
		for line in fileId:
      if line[0] == '#':
        print('Encountered comment')
      else:
				#scanner.processInputLine(line)

# Run the Scanner Parser.
def runScannerParser(inputFile, outputFile):
	inputFilePath) = os.path.join(os.path.dirname(os.path.abspath(__file__)), inputFile)
	outputFilePath = os.path.join(os.path.dirname(os.path.abspath(__file__)), outputFile)
  processInputFile(inputFilePath)
	
		
# Begin code execution here.
if __name__ == "__main__":
	runScannerParser('HW2-Input.txt', 'output.txt')
	










