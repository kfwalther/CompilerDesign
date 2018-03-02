'''

@author: Kevin Walther
@class: Compiler Design 605.415 Spring 2018
@assignment: Homework 2
@brief: This class defines a Scanner/Parser object.

'''

import re
import Cell
import Token
import ParseTreeNode
import Spreadsheet


mathematicalOperators = ['+', '-', '*', '/']

# Define a class to hold and track the stateful scanner information.
class ScannerParser():
    def __init__(self):
        self.tempEquation = []
        self.tempCellLocation = None
        self.tempControllerList = []
        self.regexIntFloatFormat = re.compile('[-]?\d+(\.\d*([Ee][- ]?\d+)?)?')
        # Initialize the spreadsheet with empty cells.
        self.spreadsheet = Spreadsheet.Spreadsheet()

    # Define a method to verify that the input value matches the format of an int or float.
    def verifyIntOrFloat(self, inputVal):
        tempResult = self.regexIntFloatFormat.match(inputVal)
        if tempResult is None:
            raise ValueError('Error occurred matching numeric value ' + inputVal + ' to int/float format.')
        if ('.' in inputVal) or ('e' in inputVal) or ('E' in inputVal):
            return Cell.ValueType.FLOAT
        else:
            return Cell.ValueType.INT
        
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
        if self.spreadsheet.isId(self.tempEquation[0]) == 'id':
            tempNode.childNode = Token.Token(Token.TokenType.ID, self.tempEquation[0])
            # Add this cell ID to the controller list.
            if self.tempEquation[0] not in self.tempControllerList:
                self.tempControllerList.append(self.tempEquation[0])
            # Also add the equation cell to the user list of the referenced ID cell.
            if self.tempCellLocation not in self.spreadsheet.getCell(self.tempEquation[0]).userList:
                self.spreadsheet.getCell(self.tempEquation[0]).userList.append(self.tempCellLocation)
            # Get the value of this cell, to calculate the equation.
            tempVal = self.spreadsheet.getCell(self.tempEquation[0]).getCellValue()
            tempNode.childNode.tokenValueType = self.spreadsheet.getCell(self.tempEquation[0]).cellValueType
            if tempVal:
                tempNode.calculatedValue = tempVal
            else:
                tempNode.calculatedValue = 'ERROR'
            self.matchToken(self.tempEquation[0])
        # Check for a numeric token.
        elif self.tempEquation[0].isnumeric():
            tempNode.childNode = Token.Token(Token.TokenType.NUM, self.tempEquation[0])
            tempNode.calculatedValue = tempNode.childNode.tokenValue
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
            raise ValueError('Error occurred when parsing expression for cell: ' + self.tempCellLocation)
        return tempNode
    
    # Define a method to perform the recursive descent parsing.
    def recursiveDescentParse(self):
        self.tempEquation = self.tempEquation[2:]
        rootNode = self.parseEquationRule()
        return rootNode
        
    # Define a method to trim the parse tree down to a abstract syntax tree.
    def trimParseTree(self, curNode):
        # Check for a Token child node.
        if type(curNode.childNode) is Token.Token:
            # Check if we have reached a leaf node (ID or NUM Token)
            if curNode.childNode.tokenType in [Token.TokenType.NUM, Token.TokenType.ID]:
                # Place the token on the current node.
                # Return the token up a level.
                curNode.token = curNode.childNode
                return curNode.childNode
            # Otherwise, we have an operator (and left/right nodes to process).
            else:
                return curNode.childNode
        # Check the left node.
        if (curNode.leftNode != None) and (curNode.nodeType is not ParseTreeNode.NodeType.PARENEXP):
            curNode.leftNode = self.trimParseTree(curNode.leftNode)
        # Check the center node.
        if curNode.childNode != None:
            # Save the entity returned from below.
            returnedEntity = self.trimParseTree(curNode.childNode)
            # Check if ParseTreeNode or Token was returned.
            if type(returnedEntity) is ParseTreeNode.ParseTreeNode:
                # We are in a ParseTreeNode column, return the ParseTreeNode.
                return returnedEntity
            # Otherwise a token was returned, so save it in the current node.
            else:
                curNode.token = returnedEntity
                curNode.childNode = None
        # Check the right node.
        if (curNode.rightNode != None) and (curNode.nodeType is not ParseTreeNode.NodeType.PARENEXP):
            self.trimParseTree(curNode.rightNode)
        # Return the current node with its three children.
        return curNode        

    # Define a method to parse the AST and calculate it's new value.
    def calculateAST(self, curNode):
        # Check for a leaf node (ID or NUM).
        if curNode.leftNode == None:
            # We are at a leaf node, get the number or value from another Cell.
            if curNode.token.tokenType == Token.TokenType.ID:
                # Get the latest Cell value and update calculated value.
                curNode.calculatedValue = self.spreadsheet.getCell(curNode.token.tokenValue).getCellValue()
                curNode.calculatedValueType = self.spreadsheet.getCell(curNode.token.tokenValue).CellValueType
            elif curNode.token.tokenType == Token.TokenType.NUM:
                # Get the number value and update calculated value.
                curNode.calculatedValue = curNode.token.tokenValue
                curNode.calculatedValueType = curNode.token.tokenValueType
            return
        # Otherwise, perform depth first search traversal.
        else:
            # Calculate the left node.
            self.calculateAST(curNode.leftNode)
            # Calculate the right node.
            self.calculateAST(curNode.rightNode)
            # Perform the operation for the given operator and operand types.
            
        # Return the current node with its three children.
        return curNode        
    
    # Define a method to process a single line of input from the file.
    def processLine(self, inputLine):
        # Check if the line is empty.
        if not inputLine.strip():
            return
        # Identify text cells. 
        if '"' in inputLine:
            self.spreadsheet.getCell(inputLine.split('"')[0].strip()).cellType = Cell.CellType.TEXT
            self.spreadsheet.getCell(inputLine.split('"')[0].strip()).cellValue = inputLine.split('"')[1]
        # Identify expression cells.
        elif '=' in inputLine:
            # Perform recursive descent parse.
            self.tempEquation = [elem.strip() for elem in re.split('(=|\\(|\\)|\\+|\\-|\\*|\\/)',inputLine) if elem.strip() != '']
            self.tempCellLocation = self.tempEquation[0]
            self.tempControllerList = []
            rootNode = self.recursiveDescentParse()
            print('Printing parse tree at cell: ' + self.tempCellLocation)
#             self.printTree(rootNode, 0)
            # Update the current cell with the new information from the recursive parse.
            self.spreadsheet.getCell(self.tempCellLocation).cellType = Cell.CellType.EQU
            self.spreadsheet.getCell(self.tempCellLocation).cellValue = rootNode.calculatedValue
            self.spreadsheet.getCell(self.tempCellLocation).parseTree = rootNode
            self.spreadsheet.getCell(self.tempCellLocation).controllerList = self.tempControllerList
            # Recalculate the values in parse tree based on this updated cell.
#             [self.recalculateValue(userCell) for userCell in self.spreadsheet.getCell(self.tempCellLocation).userList]
            # Trim the tree and print it.
            syntaxTreeRoot = self.trimParseTree(rootNode)
            self.printTruncatedTree(syntaxTreeRoot, 0)
        # Identify the numeric cells.
        else:
            curLine = inputLine.split()
            if len(curLine) == 2:
                # Verify that the input is int/float format.
                self.spreadsheet.getCell(curLine[0]).cellType = Cell.CellType.NUM
                self.spreadsheet.getCell(curLine[0]).cellValueType = self.verifyIntOrFloat(curLine[1])
                if self.spreadsheet.getCell(curLine[0]).cellValueType == Cell.ValueType.INT:
                    self.spreadsheet.getCell(curLine[0]).cellValue = int(curLine[1])
                else:
                    self.spreadsheet.getCell(curLine[0]).cellValue = float(curLine[1])
                # Recalculate the values in parse tree based on this updated cell.
#                 [self.recalculateValue(userCell) for userCell in self.spreadsheet.getCell(curLine[0]).userList]
            else:
                raise ValueError('Input line does not match expected format: ' + inputLine)
        
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

    # Define a function to perform a depth first search to print the abstract syntax tree.
    def printTruncatedTree(self, tempNode, depth):
        print((depth * '\t') + '*** SYNTAX TREE NODE ***')
        if tempNode.token is not None:
            print((depth * '\t') + 'Token: ' + str(tempNode.token.tokenValue))
        if tempNode.leftNode != None and type(tempNode.leftNode) is ParseTreeNode.ParseTreeNode:
            self.printTruncatedTree(tempNode.leftNode, depth + 1)
        if tempNode.rightNode != None and type(tempNode.rightNode) is ParseTreeNode.ParseTreeNode:
            self.printTruncatedTree(tempNode.rightNode, depth + 1)





