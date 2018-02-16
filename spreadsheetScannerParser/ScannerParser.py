'''

@author: Kevin Walther
@class: Compiler Design 605.415 Spring 2018
@assignment: Homework 2
@brief: This Python program implements a spreadsheet scanner parser.

'''

import os
import sys


indexToColDict = {
    1: 'A',
    2: 'B',       
    3: 'C',
    4: 'D',
    5: 'E',
    6: 'F'       
}

mathematicalOperators = ['+', '-', '*', '/']

# Define a class to hold and track the stateful scanner information.
class Scanner():
    def __init__(self):
        self.tokenDict = {}
        
    # Write a function to validate the token ID names (e.g. A0, C4, etc.)
    def verifyId(self, id):
        if len(id) != 2:
            raise ValueError('Malformed ID value detected: ' + id)
        col, row = id
        if (not col.isalpha()) or (not row.isnumeric()):
            raise ValueError('Malformed ID value detected:' + id)
        if (ord(col) not in range(ord('A'), ord('G'))):
            raise ValueError('Invalid column reference in ID (columns must be within range A to F):' + col)
        if (int(row) not in range(0, 10)):
            raise ValueError('Invalid row reference in ID (rows must be within range 0 to 9):' + row)
        return id
    
    # Define a method to process a single line of input from the file.
    def processLine(self, inputLine):
        # Check if the line is empty.
        if not inputLine.strip():
            return
        # Identify text IDs. 
        if '"' in inputLine:
            self.tokenDict[self.verifyId(inputLine.split('"')[0].strip())] = inputLine.split('"')[1]
        # Identify expressions.
        elif '=' in inputLine:
            curOperator = [op for op in mathematicalOperators if op in inputLine][0]
            expressionComponents = list(inputLine.partition('='))
            expressionComponents = expressionComponents[0:-1] + list(expressionComponents[-1].partition(curOperator))
            expressionComponents = [elem.strip() for elem in expressionComponents]
            self.tokenDict[self.verifyId(expressionComponents[0])] = self.performMathematicalOperation(expressionComponents)
        else:
            curLine = inputLine.split()
            if len(curLine) == 2:
                self.tokenDict[self.verifyId(curLine[0])] = int(curLine[1])
            else:
                raise ValueError('Input line does not match expected format: ' + inputLine)
        
    # Define a method to perform a mathematical operation on two operands and return the result.
    def performMathematicalOperation(self, curLine):
        # Verify the operands exist and that they are numeric operands.
        if (curLine[2] in self.tokenDict.keys() and isinstance(self.tokenDict[curLine[2]], int) and 
                curLine[4] in self.tokenDict.keys() and isinstance(self.tokenDict[curLine[4]], int)): 
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
            return 'ERROR'
        
    # Define a method to print an individual cell of data.
    def printCell(self, cellContents):
        return str(cellContents) + (' ' * (8 - len(str(cellContents)))) + '|'
        
    # Define a method to print out the parsed tokens.
    def printTokens(self):
        [print(tokenName + ', ' + str(tokenValue)) for tokenName, tokenValue in self.tokenDict.items()]
        # Print out the grid, or spreadsheet.
        print('|' + self.printCell('') + self.printCell('A') + self.printCell('B') + 
                self.printCell('C') + self.printCell('D') + self.printCell('E') + self.printCell('F'))
        for row in range(0,10):
            tempStr = '|' + self.printCell(row)
            for col in range(1,7):
                tempStr = tempStr + self.printCell(self.tokenDict.get((indexToColDict[col] + str(row)), ''))
            print(tempStr)
            
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











