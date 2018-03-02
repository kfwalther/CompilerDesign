'''

@author: Kevin Walther
@class: Compiler Design 605.415 Spring 2018
@assignment: Homework 2
@brief: This class defines a Token.

'''

from enum import Enum
import Cell 

class TokenType(Enum):
	ID = 1
	NUM = 2
	SPECIAL = 3
	
class Token():
	def __init__(self, tokenType=None, tokenValue=None, tokenValueType=None):
		self.tokenType, self.tokenValueType = tokenType, tokenValueType
		# Keep track if a NUM Token is Int or Float type.
		if self.tokenType == TokenType.NUM:
		if self.tokenValueType == Cell.ValueType.INT:
			self.tokenValue = int(tokenValue)
		else:
			self.tokenValue = float(tokenValue)
		
	def printToken(self, treeDepth):
		print((treeDepth * '\t') + '*** TOKEN NODE ***')
		print((treeDepth * '\t') + 'TokenType: ' + str(self.tokenType.name))
		print((treeDepth * '\t') + 'TokenValue: ' + str(self.tokenValue))







