'''

@author: Kevin Walther
@class: Compiler Design 605.415 Spring 2018
@assignment: Homework 3
@brief: This class defines a node of the Parse Tree used during recursive descent parsing.

'''

from enum import Enum

class NodeType(Enum):
    EQU = 1,
    ADDOP = 2,
    TERM = 3,
    MULTOP = 4,
    FACTOR = 5,
    PARENEXP = 6    
    
class ParseTreeNode():
    def __init__(self):
        self.token, self.nodeType, self.rightNode, self.childNode, self.leftNode, self.calculatedValue = None, None, None, None, None, None
        
    def calculateValue(self):
        if self.leftNode.calculatedValue == 'ERROR' or self.rightNode.calculatedValue == 'ERROR':
            self.calculatedValue = 'ERROR'
            return
        if self.childNode.childNode.tokenValue == '+':
            print(str(self.leftNode.calculatedValue) + ' + ' + str(self.rightNode.calculatedValue))
            self.calculatedValue = self.leftNode.calculatedValue + self.rightNode.calculatedValue
        elif self.childNode.childNode.tokenValue == '-':
            print(str(self.leftNode.calculatedValue) + ' - ' + str(self.rightNode.calculatedValue))
            self.calculatedValue = self.leftNode.calculatedValue - self.rightNode.calculatedValue
        elif self.childNode.childNode.tokenValue == '*':
            print(str(self.leftNode.calculatedValue) + ' * ' + str(self.rightNode.calculatedValue))
            self.calculatedValue = self.leftNode.calculatedValue * self.rightNode.calculatedValue
        elif self.childNode.childNode.tokenValue == '/':
            print(str(self.leftNode.calculatedValue) + ' / ' + str(self.rightNode.calculatedValue))
            self.calculatedValue = self.leftNode.calculatedValue / self.rightNode.calculatedValue


    