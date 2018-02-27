'''

@author: Kevin Walther
@class: Compiler Design 605.415 Spring 2018
@assignment: Homework 2
@brief: This class defines a Scanner.

'''

import re
import Cell
import Token
import ParseTreeNode

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
dfaColsMap =    {'+':0,  '-':1,  '*':2,  '/':3,  '(':4,   ')':5,   'id':6,  'other':7}
dfa = {      #   '+'     '-'     '*'     '/'     '('      ')'      'id'     'other'
    'NUM':      ['OP',   'OP',   'OP',   'OP',   'ERROR', 'RPAREN','ERROR', 'ERROR'],
    'OP':       ['ERROR','ERROR','ERROR','ERROR','LPAREN','ERROR', 'NUM',   'ERROR'],
    'LPAREN':   ['ERROR','ERROR','ERROR','ERROR','LPAREN','ERROR', 'NUM',   'ERROR'],
    'RPAREN':   ['OP',   'OP',   'OP',   'OP',   'ERROR', 'RPAREN','ERROR', 'ERROR'],
    'ERROR':    []
}

mathematicalOperators = ['+', '-', '*', '/']

# Define a class to hold and track the stateful scanner information.
class Scanner():
    def __init__(self):
        self.spreadsheetDict = {}
        self.tempEquation = []
        self.tempCellLocation = None
        self.tempControllerList = []
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
    
    # Define a method to retrieve the contents of a particular cell.
    def getCell(self, cellLocation):
        return self.spreadsheetDict[self.verifyId(cellLocation)].getCellValue()
    
    # Define a method to clear the contents of a particular cell.
    def clearCell(self, cellLocation):
        self.spreadsheetDict[self.verifyId(cellLocation)] = Cell.Cell()
    
    # Define a method to match the current equation component to a value, and move to the next value.
    def matchToken(self, tokenVal):
        if self.tempEquation[0] == tokenVal:
            self.tempEquation = self.tempEquation[1:]
        else:
            raise ValueError('Error occurred matching token ' + tokenVal + ' to ' + self.tempEquation[0])

    # Define a method to parse an equation rule, which uses repetion.
    def parseEquationRule(self):
        tempNode = ParseTreeNode.ParseTreeNode()
        # Parse a term.
        tempNode.childNode = self.parseTermRule()
        tempNode.calculatedValue = tempNode.childNode.calculatedValue
        while self.tempEquation and (self.tempEquation[0] in ['+','-']):
            # Push the parsed term down and to the left in the tree.
            newParent = ParseTreeNode.ParseTreeNode()
            tempNode.nodeType = ParseTreeNode.NodeType.EQU
            newParent.leftNode = tempNode
            tempNode = newParent
            tempNode.calculatedValue = tempNode.leftNode.calculatedValue
            tempNode.childNode = self.parseAddOperatorRule()
            tempNode.rightNode = self.parseTermRule()
            tempNode.calculateValue()
        tempNode.nodeType = ParseTreeNode.NodeType.EQU
        # Calculate this node's value.
        if tempNode.childNode.nodeType == ParseTreeNode.NodeType.TERM:
            tempNode.calculatedValue = tempNode.childNode.calculatedValue
        else:
            tempNode.calculateValue()   
        # TODO: Should we swap the ordering here?
        return tempNode
    
    # Define a method to parse an term rule, which uses repetion.
    def parseTermRule(self):
        tempNode = ParseTreeNode.ParseTreeNode()
        newParent = None
        # Parse a factor.
        tempNode.childNode = self.parseFactorRule()
        tempNode.calculatedValue = tempNode.childNode.calculatedValue
        # Check if the first element is mult operator
        while self.tempEquation and (self.tempEquation[0] in ['*','/']):
            # Push the parsed factor down and to the left in the tree.
            newParent = ParseTreeNode.ParseTreeNode()
            tempNode.nodeType = ParseTreeNode.NodeType.TERM
            newParent.leftNode = tempNode
            tempNode = newParent
            tempNode.calculatedValue = tempNode.leftNode.calculatedValue
            tempNode.childNode = self.parseMultOperatorRule()
            tempNode.rightNode = self.parseFactorRule()
            tempNode.calculateValue()
        tempNode.nodeType = ParseTreeNode.NodeType.TERM
        # Calculate this node's value.
        if tempNode.childNode.nodeType == ParseTreeNode.NodeType.FACTOR:
            tempNode.calculatedValue = tempNode.childNode.calculatedValue
        else:
            tempNode.calculateValue()
        # TODO: Should we swap the ordering here?
        return tempNode
    
    def parseFactorRule(self):
        tempNode = ParseTreeNode.ParseTreeNode()
        # Check for an ID token.
        if self.isId(self.tempEquation[0]) == 'id':
            tempNode.childNode = Token.Token(Token.TokenType.ID, self.tempEquation[0])
            # Add this cell ID to the controller list.
            if self.tempEquation[0] not in self.tempControllerList:
                self.tempControllerList.append(self.tempEquation[0])
            # Also add the equation cell to the user list of the referenced ID cell.
            if self.tempCellLocation not in self.spreadsheetDict[self.tempEquation[0]].userList:
                self.spreadsheetDict[self.tempEquation[0]].userList.append(self.tempCellLocation)
            # Get the value of this cell, to calculate the equation.
            tempVal = self.getCell(self.tempEquation[0])
            if tempVal.isnumeric():
                tempNode.calculatedValue = tempVal
            else:
                tempNode.calculatedValue = 'ERROR'
            self.matchToken(self.tempEquation[0])
        # Check for a numeric token.
        elif self.tempEquation[0].isnumeric():
            tempNode.childNode = Token.Token(Token.TokenType.NUM, int(self.tempEquation[0]))
            tempNode.calculatedValue = int(self.tempEquation[0])
            self.matchToken(self.tempEquation[0])
        # Otherwise, we have to parse a parenthesized expression.
        else:
            tempNode.childNode = self.parseParenExpressionRule()
            tempNode.calculatedValue = tempNode.childNode.calculatedValue
        tempNode.nodeType = ParseTreeNode.NodeType.FACTOR
        return tempNode
    
    # Define a function to parse the multiply/divide operators. 
    def parseMultOperatorRule(self):
        tempNode = ParseTreeNode.ParseTreeNode()
        tempNode.nodeType = ParseTreeNode.NodeType.MULTOP
        tempNode.childNode = Token.Token(Token.TokenType.SPECIAL, self.tempEquation[0])
        if self.tempEquation[0] == '*':
            self.matchToken('*')
        else:
            self.matchToken('/')
        return tempNode
        
    # Define a function to parse the add/subtract operators. 
    def parseAddOperatorRule(self):
        tempNode = ParseTreeNode.ParseTreeNode()
        tempNode.nodeType = ParseTreeNode.NodeType.ADDOP
        tempNode.childNode = Token.Token(Token.TokenType.SPECIAL, self.tempEquation[0])
        if self.tempEquation[0] == '+':
            self.matchToken('+')
        else:
            self.matchToken('-')
        return tempNode
    
    def parseParenExpressionRule(self):
        tempNode = ParseTreeNode.ParseTreeNode()
        # Check for an opening parenthesis.
        if self.tempEquation[0] == '(':
            tempNode.nodeType = ParseTreeNode.NodeType.PARENEXP
            tempNode.leftNode = Token.Token(Token.TokenType.SPECIAL, self.tempEquation[0])
            self.matchToken('(')
            # Call the function to parse an equation.
            tempNode.childNode = self.parseEquationRule()
            tempNode.calculatedValue = tempNode.childNode.calculatedValue
            tempNode.rightNode = Token.Token(Token.TokenType.SPECIAL, self.tempEquation[0])
            self.matchToken(')')
        else:
            raise ValueError('Error in parseParenExpressionRule, no parenthesized equation found!')
        return tempNode
    
    # Define a method to perform the recursive descent parsing.
    def recursiveDescentParse(self):
        self.tempEquation = self.tempEquation[2:]
        rootNode = self.parseEquationRule()
        return rootNode
    
    # Define a method to process a single line of input from the file.
    def processLine(self, inputLine):
        # Check if the line is empty.
        if not inputLine.strip():
            return
        # Identify text cells. 
        if '"' in inputLine:
            self.spreadsheetDict[self.verifyId(inputLine.split('"')[0].strip())].cellType = Cell.CellType.TEXT
            self.spreadsheetDict[self.verifyId(inputLine.split('"')[0].strip())].cellValue = inputLine.split('"')[1]
        # Identify expression cells.
        elif '=' in inputLine:
            # Perform recursive descent parse.
            self.tempEquation = [elem.strip() for elem in re.split('(=|\\(|\\)|\\+|\\-|\\*|\\/)',inputLine) if elem.strip() != '']
            self.tempCellLocation = self.tempEquation[0]
            self.tempControllerList = []
            rootNode = self.recursiveDescentParse()
            print('Printing parse tree at cell: ' + self.tempCellLocation)
            self.printTree(rootNode, 0)
            # Create a new cell to house the evaluated expression and its components.
            self.spreadsheetDict[self.tempCellLocation].cellType = Cell.CellType.EQU
            self.spreadsheetDict[self.tempCellLocation].cellValue = rootNode.calculatedValue
            self.spreadsheetDict[self.tempCellLocation].controllerList = self.tempControllerList
        # Identify the numeric cells.
        else:
            curLine = inputLine.split()
            if len(curLine) == 2:
                self.spreadsheetDict[self.verifyId(curLine[0])].cellType = Cell.CellType.NUM
                self.spreadsheetDict[self.verifyId(curLine[0])].cellValue = int(curLine[1])
            else:
                raise ValueError('Input line does not match expected format: ' + inputLine)
        
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

    # Define a function to perform a depth first search to print the parse tree.
    def printTree(self, tempNode, depth):
        print((depth * '\t') + '*** PARSE TREE NODE ***')
        print((depth * '\t') + 'NodeType: ' + str(tempNode.nodeType.name))
        if tempNode.leftNode != None and type(tempNode.leftNode) is ParseTreeNode.ParseTreeNode:
            self.printTree(tempNode.leftNode, depth + 1)
        if tempNode.childNode != None and type(tempNode.childNode) is ParseTreeNode.ParseTreeNode:
            self.printTree(tempNode.childNode, depth + 1)
        if tempNode.rightNode != None and type(tempNode.rightNode) is ParseTreeNode.ParseTreeNode:
            self.printTree(tempNode.rightNode, depth + 1)
        if tempNode.leftNode != None and type(tempNode.leftNode) is Token.Token:
            tempNode.leftNode.printToken(depth + 1)
        if tempNode.childNode != None and type(tempNode.childNode) is Token.Token:
            tempNode.childNode.printToken(depth + 1)
        if tempNode.rightNode != None and type(tempNode.rightNode) is Token.Token:
            tempNode.rightNode.printToken(depth + 1)
  
    # Define a method to print out the controller and user lists for each cell.
    def printCellInfo(self):
        for row in range(0,10):
            for col in range(1,7):
                tempCell = self.spreadsheetDict.get((indexToColDict[col] + str(row)), '')
                print('Cell ' + indexToColDict[col] + str(row) + ' Controller List: ')
                if len(tempCell.controllerList) == 0:
                    print('emtpy')
                else:
                    [print(tempController) for tempController in tempCell.controllerList] 
                print('Cell ' + indexToColDict[col] + str(row) + ' User List: ')
                if len(tempCell.userList) == 0:
                    print('emtpy')
                else:
                    [print(tempUser) for tempUser in tempCell.userList]




