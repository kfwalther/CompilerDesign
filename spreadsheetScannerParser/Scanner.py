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
    
#     # Define a method to perform a table-driven parse of an expression and store it in a cell.
#     def parseExpression(self, rightExpression):
#         # Get the first instance of a mathematical operator, and partition the expression around it.
#         expressionComponents = [elem.strip() for elem in re.split('(\\(|\\)|\\+|\\-|\\*|\\/)',rightExpression)]
#         expressionComponents = [exp for exp in expressionComponents if exp]
#         
#         expressionComponents.append('other')
#         expressionCounter = 0
#         dfaState = dfa['OP'][dfaColsMap[self.isId(expressionComponents[expressionCounter])]]
#         tokenList = []
#         # Make a better exit case for more complex expressions...
#         while ((dfaState is not 'ERROR') and (expressionCounter < 3)):
#             if dfaState in ['NUM', 'RPAREN']:
#                 # Add this ID token to the list.
#                 tokenList.append(Token.Token(Token.TokenType.ID, expressionComponents[expressionCounter]))
#                 # Update the state.
#                 expressionCounter += 1
#                 #TODO: Make a function here to check for operator or 'other'
#                 dfaState = dfa[dfaState][dfaColsMap[expressionComponents[expressionCounter]]]
#             elif dfaState in ['OP', 'LPAREN']:
#                 # Add this special character token to the list.
#                 tokenList.append(Token.Token(Token.TokenType.SPECIAL, expressionComponents[expressionCounter]))
#                 # Update the state.
#                 expressionCounter += 1
#                 dfaState = dfa[dfaState][dfaColsMap[self.isId(expressionComponents[expressionCounter])]]
#         return tokenList
#     

    def matchToken(self, tokenVal):
        if self.tempEquation[0] == tokenVal:
            self.tempEquation = self.tempEquation[1:]
        else:
            raise ValueError('Error occurred matching token ' + tokenVal + ' to ' + self.tempEquation[0])

    def parseEquationRule(self):
        tempNode = ParseTreeNode.ParseTreeNode()
        # Parse a term.
        tempNode.childNode = self.parseTermRule()
        while self.tempEquation and (self.tempEquation[0] in ['+','-']):
            # Push the parsed term down and to the left in the tree.
            newParent = ParseTreeNode.ParseTreeNode()
            tempNode.nodeType = ParseTreeNode.NodeType.EQU
            newParent.leftNode = tempNode
            tempNode = newParent
            tempNode.childNode = self.parseAddOperatorRule()
            tempNode.rightNode = self.parseTermRule()
        tempNode.nodeType = ParseTreeNode.NodeType.EQU
        # TODO: Should we swap the ordering here?
        return tempNode
    
    def parseTermRule(self):
        tempNode = ParseTreeNode.ParseTreeNode()
        newParent = None
        # Parse a factor.
        tempNode.childNode = self.parseFactorRule()
        # Check if the first element is mult operator
        while self.tempEquation and (self.tempEquation[0] in ['*','/']):
            # Push the parsed factor down and to the left in the tree.
            newParent = ParseTreeNode.ParseTreeNode()
            tempNode.nodeType = ParseTreeNode.NodeType.TERM
            newParent.leftNode = tempNode
            tempNode = newParent
            tempNode.childNode = self.parseMultOperatorRule()
            tempNode.rightNode = self.parseFactorRule()
        tempNode.nodeType = ParseTreeNode.NodeType.TERM
        # TODO: Should we swap the ordering here?
        return tempNode
    
    def parseFactorRule(self):
        tempNode = ParseTreeNode.ParseTreeNode()
        # Check for an ID token.
        if self.isId(self.tempEquation[0]) == 'id':
            tempNode.childNode = Token.Token(Token.TokenType.ID, self.tempEquation[0])
            self.matchToken(self.tempEquation[0])
        elif self.tempEquation[0].isnumeric():
            tempNode.childNode = Token.Token(Token.TokenType.NUM, self.tempEquation[0])
            self.matchToken(self.tempEquation[0])
        else:
            tempNode.childNode = self.parseParenExpressionRule()
        tempNode.nodeType = ParseTreeNode.NodeType.FACTOR
        return tempNode
    
    def parseMultOperatorRule(self):
        tempNode = ParseTreeNode.ParseTreeNode()
        tempNode.nodeType = ParseTreeNode.NodeType.MULTOP
        tempNode.childNode = Token.Token(Token.TokenType.SPECIAL, self.tempEquation[0])
        if self.tempEquation[0] == '*':
            self.matchToken('*')
        else:
            self.matchToken('/')
        return tempNode
        
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
        if self.tempEquation[0] == '(':
            tempNode.nodeType = ParseTreeNode.NodeType.PARENEXP
            tempNode.leftNode = Token.Token(Token.TokenType.SPECIAL, self.tempEquation[0])
            self.matchToken('(')
            tempNode.childNode = self.parseEquationRule()
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
            # Perform recursive descent parse.
            self.tempEquation = [elem.strip() for elem in re.split('(=|\\(|\\)|\\+|\\-|\\*|\\/)',inputLine) if elem.strip() != '']
            tempId = self.tempEquation[0]
            rootNode = self.recursiveDescentParse()
            self.printTree(rootNode, 0)
            # Isolate the right side of the expression, and parse that portion.
#             expressionComponents = list(inputLine.partition('='))
#             tokenList = self.parseExpression(expressionComponents[-1])
            # Create a new cell to house the evaluated expression and its components.
            self.spreadsheetDict[tempId] = Cell.Cell(Cell.CellType.TOKEN, 0, rootNode)
        # Identify the numeric cells.
        else:
            curLine = inputLine.split()
            if len(curLine) == 2:
                self.spreadsheetDict[self.verifyId(curLine[0])] = Cell.Cell(Cell.CellType.NUM, int(curLine[1]))
            else:
                raise ValueError('Input line does not match expected format: ' + inputLine)

    # Define a method to perform a mathematical operation on two operands and return the result.
    def performMathematicalOperation(self, tokenList):
        # Check if this expression contains parentheses, we don't support these yet.
        if (len([curToken for curToken in tokenList if curToken.tokenValue in ['(',')']]) > 0) or (len(tokenList) == 0):                
            return 'ERROR'
        # Verify the operands exist and that they are numeric operands.
        else:
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
                    # Encountered unsupported mathematical operator. Return 'ERROR' cell.
                    return 'ERROR'
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
            






