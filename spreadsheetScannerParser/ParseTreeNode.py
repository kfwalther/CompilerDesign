'''

@author: Kevin Walther
@class: Compiler Design 605.415 Spring 2018
@assignment: Homework 3
@brief: This class defines a node of the Parse Tree used during recursive descent parsing.

'''

from enum import Enum
import Cell

class NodeType(Enum):
    EQU = 1,
    ADDOP = 2,
    TERM = 3,
    MULTOP = 4,
    FACTOR = 5,
    PARENEXP = 6    
    
class ParseTreeNode():
    def __init__(self):
        self.token, self.nodeType, self.rightNode, self.childNode, self.leftNode, self.calculatedValue, self.calculatedValueType = None, None, None, None, None, None, None
        
    # Define a method to calculate the value for a parse tree node, given its operands and operator type.
    def calculateValue(self):
        # Check for ERROR cells.
        if self.leftNode.calculatedValue == 'ERROR' or self.rightNode.calculatedValue == 'ERROR':
            self.calculatedValue = 'ERROR'
            return
        # Check what the calculated value type should be (int or float or promote to float).
        if Cell.ValueType.FLOAT in [self.leftNode.calculatedValueType, self.rightNode.calculatedValueType]:
            # Both floats or promote to float.
            self.calculatedValueType = Cell.ValueType.FLOAT
            self.performMathematicalOperation(float(self.leftNode.calculatedValue), float(self.rightNode.calculatedValue))
        else:
            self.calculatedValueType = Cell.ValueType.INT
            self.performMathematicalOperation(int(self.leftNode.calculatedValue), int(self.rightNode.calculatedValue))
        
    # Define a method to perform the mathematical operation on two operands.
    def performMathematicalOperation(self, leftVal, rightVal):
        # Check if we are operating on Parse Tree or AST.
        if self.childNode is not None:
            # Perform the specific operation within the ParseTree.
            if self.childNode.childNode.tokenValue == '+':
                self.calculatedValue = leftVal + rightVal
            elif self.childNode.childNode.tokenValue == '-':
                self.calculatedValue = leftVal - rightVal
            elif self.childNode.childNode.tokenValue == '*':
                self.calculatedValue = leftVal * rightVal
            elif self.childNode.childNode.tokenValue == '/':
                self.calculatedValue = leftVal / rightVal
        else:
            # Perform the specific operation within the AST.
            if self.token.tokenValue == '+':
                self.calculatedValue = leftVal + rightVal
            elif self.token.tokenValue == '-':
                self.calculatedValue = leftVal - rightVal
            elif self.token.tokenValue == '*':
                self.calculatedValue = leftVal * rightVal
            elif self.token.tokenValue == '/':
                self.calculatedValue = leftVal / rightVal  

    