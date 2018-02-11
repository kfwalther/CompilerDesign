'''

@author: Kevin Walther
@class: Compiler Design 605.415 Spring 2018
@assignment: Homework 2
@brief: This Python program implements a spreadsheet scanner parser.

'''

import os
import sys


mathematicalOperators = ['+', '-', '*', '/']

# Define a class to hold and track the stateful scanner information.
class Scanner():
    def __init__(self):
        self.tokenDict = {}
        
    # Define a method to process a single line of input from the file.
    def processLine(self, inputLine):
        # Identify text IDs. 
        if '"' in inputLine:
            self.tokenDict[inputLine.split('"')[0].strip()] = inputLine.split('"')[1]
        else:
            curLine = inputLine.split()
            if len(curLine) == 2:
                self.tokenDict[curLine[0]] = curLine[1]
            # Process an equation.
            elif len(curLine) == 5:
                # Verify it has valid syntax.
                if ((curLine[1] == '=') and (curLine[3] in mathematicalOperators)):
                    self.performMathematicalOperation(curLine)
            else:
                raise ValueError('Input line does not match expected format: ' + inputLine)
        
    # Define a method to perform a mathematical operation on two operands and return the result.
    def performMathematicalOperation(self, curLine):
        if curLine[2] in self.tokenDict.keys() and curLine[4] in self.tokenDict.keys(): 
            if curLine[3] == '+':
                return self.tokenDict[curLine[2]] + self.tokenDict[curLine[4]]
            elif curLine[3] == '-':
                return self.tokenDict[curLine[2]] - self.tokenDict[curLine[4]]
            elif curLine[3] == '*':
                return self.tokenDict[curLine[2]] * self.tokenDict[curLine[4]]
            elif curLine[3] == '/':
                return self.tokenDict[curLine[2]] / self.tokenDict[curLine[4]]                                    
            else:
                raise ValueError('Unsupported mathematical operator: ' + curLine[3])
        else:
            raise ValueError('Cannot perform operation on an ID not yet defined: ' + curLine[2] + ', ' + curLine[4])
        
    # Define a method to print out the parsed tokens.
    def printTokens(self):
        for tokenName, tokenValue in self.tokenDict.items():
            print(tokenName + ', ' + tokenValue)
            
# Process the input file line by line.
def processInputFile(inputFilePath):
    with open(inputFilePath, 'r') as fileId:
        scanner = Scanner()
        for line in fileId:
            if line[0] == '#':
                print('Encountered comment')
            else:
                try:
                    print(line)
                    scanner.processLine(line)
                except ValueError as err:
                    print('WARNING: ' + err.args[0])
                    continue
        scanner.printTokens()

# Run the Scanner Parser.
def runScannerParser(inputFile, outputFile):
    inputFilePath = os.path.join(os.path.dirname(os.path.abspath(__file__)), inputFile)
    outputFilePath = os.path.join(os.path.dirname(os.path.abspath(__file__)), outputFile)
    processInputFile(inputFilePath)
    

# Begin code execution here.
if __name__ == "__main__":
    runScannerParser('HW2-Input.txt', 'output.txt')











