'''

@author: Kevin Walther
@class: Compiler Design 605.415 Spring 2018
@assignment: Homework 2
@brief: This Python program implements a spreadsheet scanner parser.

'''

import os
import sys
import Cell
import Token

indexToColDict = {
    1: 'A',
    2: 'B',       
    3: 'C',
    4: 'D',
    5: 'E',
    6: 'F'       
}

# Define an array to codify our state machine states and transitions.
# To be truly table-based, we should add all the possible IDs to this table (60 IDs).
dfaColsMap =    {'+':0,  '-':1,  '*':2,  '/':3,  'id':4, 'other':5}
dfa = {      #   '+'     '-'     '*'     '/'     'id'    'other'
    'NUM':      ['OP',   'OP',   'OP',   'OP',   'ERROR', 'ERROR'],
    'OP':       ['ERROR','ERROR','ERROR','ERROR','NUM',   'ERROR'],
    'ERROR':    []
}

mathematicalOperators = ['+', '-', '*', '/']

# Define a class to hold and track the stateful scanner information.
class Scanner():
    def __init__(self):
        self.spreadsheetDict = {}
        # Initialize the spreadsheet with empty cells.
        for row in range(0,10):
            for col in range(1,7):
                self.spreadsheetDict[(indexToColDict[col] + str(row))] = Cell.Cell()
                
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

    # Write a function to validate the token ID names (e.g. A0, C4, etc.)
    def isId(self, id):
        if len(id) != 2:
            return 'other'
        col, row = id
        if (not col.isalpha()) or (not row.isnumeric()):
            return 'other'
        if (ord(col) not in range(ord('A'), ord('G'))):
            return 'other'
        if (int(row) not in range(0, 10)):
            return 'other'
        return 'id'
    
    # Define a method to perform a table-driven parse of an expression and store it in a cell.
    def parseExpression(self, rightExpression):
        # Get the first instance of a mathematical operator, and partition the expression around it.
        expressionComponents = [elem.strip() for elem in (list(
                rightExpression.partition(
                        [op for op in mathematicalOperators if op in rightExpression][0])))]
        expressionComponents.append('other')
        expressionCounter = 0
        dfaState = dfa['OP'][dfaColsMap[self.isId(expressionComponents[expressionCounter])]]
        tokenList = []
        # Make a better exit case for more complex expressions...
        while ((dfaState is not 'ERROR') and (expressionCounter < 3)):
            if (dfaState is 'NUM'):
                # Add this ID token to the list.
                tokenList.append(Token.Token(Token.TokenType.ID, expressionComponents[expressionCounter]))
                # Update the state.
                expressionCounter += 1
                #TODO: Make a functions here to check for operator or 'other'
                dfaState = dfa[dfaState][dfaColsMap[expressionComponents[expressionCounter]]]
            elif (dfaState is 'OP'):
                # Add this special character token to the list.
                tokenList.append(Token.Token(Token.TokenType.SPECIAL, expressionComponents[expressionCounter]))
                # Update the state.
                expressionCounter += 1
                dfaState = dfa[dfaState][dfaColsMap[self.isId(expressionComponents[expressionCounter])]]
        return tokenList
    
    # Define a method to process a single line of input from the file.
    def processLine(self, inputLine):
        # Check if the line is empty.
        if not inputLine.strip():
            return
        # Identify text cells. 
        if '"' in inputLine:
            self.spreadsheetDict[self.verifyId(inputLine.split('"')[0].strip())] = \
                    Cell.Cell(Cell.CellType.TEXT, inputLine.split('"')[1])
        # Identify expression cells.
        elif '=' in inputLine:
            # Isolate the right side of the expression, and parse that portion.
            expressionComponents = list(inputLine.partition('='))
            tokenList = self.parseExpression(expressionComponents[-1])
            # Create a new cell to house the evaluated expression and its components.
            self.spreadsheetDict[self.verifyId(expressionComponents[0].strip())] = \
                    Cell.Cell(Cell.CellType.TOKEN, self.performMathematicalOperation(tokenList), tokenList)
        # Identify the numeric cells.
        else:
            curLine = inputLine.split()
            if len(curLine) == 2:
                self.spreadsheetDict[self.verifyId(curLine[0])] = Cell.Cell(Cell.CellType.NUM, int(curLine[1]))
            else:
                raise ValueError('Input line does not match expected format: ' + inputLine)
        
    # Define a method to perform a mathematical operation on two operands and return the result.
    def performMathematicalOperation(self, tokenList):
        # Verify the operands exist and that they are numeric operands.
        if (tokenList[0].tokenValue in self.spreadsheetDict.keys() and isinstance(self.spreadsheetDict[tokenList[0].tokenValue].cellValue, int) and 
                tokenList[2].tokenValue in self.spreadsheetDict.keys() and isinstance(self.spreadsheetDict[tokenList[2].tokenValue].cellValue, int)): 
            if tokenList[1].tokenValue == '+':
                return self.spreadsheetDict[tokenList[0].tokenValue].cellValue + self.spreadsheetDict[tokenList[2].tokenValue].cellValue
            elif tokenList[1].tokenValue == '-':
                return self.spreadsheetDict[tokenList[0].tokenValue].cellValue - self.spreadsheetDict[tokenList[2].tokenValue].cellValue
            elif tokenList[1].tokenValue == '*':
                return self.spreadsheetDict[tokenList[0].tokenValue].cellValue * self.spreadsheetDict[tokenList[2].tokenValue].cellValue
            elif tokenList[1].tokenValue == '/':
                return self.spreadsheetDict[tokenList[0].tokenValue].cellValue / self.spreadsheetDict[tokenList[2].tokenValue].cellValue                                 
            else:
                raise ValueError('Unsupported mathematical operator: ' + tokenList[1].tokenValue)
        else:
            return 'ERROR'
        
    # Define a method to print an individual cell of data.
    def printCell(self, cellContents):
        return str(cellContents) + (' ' * (8 - len(str(cellContents)))) + '|'
        
    # Define a method to print out the parsed tokens.
    def printTokens(self):
        # Print out the grid, or spreadsheet.
        print('|' + self.printCell('') + self.printCell('A') + self.printCell('B') + 
                self.printCell('C') + self.printCell('D') + self.printCell('E') + self.printCell('F'))
        for row in range(0,10):
            tempStr = '|' + self.printCell(row)
            for col in range(1,7):
                tempStr = tempStr + self.printCell(self.spreadsheetDict.get((indexToColDict[col] + str(row)), '').cellValue)
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
    










