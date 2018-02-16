'''

@author: Kevin Walther
@class: Compiler Design 605.415 Spring 2018
@assignment: Homework 2
@brief: This class defines a Token.

'''

from enum import Enum

class TokenType(Enum):
	ID = 1
	NUM = 2
	SPECIAL = 3
	
class Token():
	def __init__(self, tokenType=None, tokenValue=None):
		self.tokenType, self.tokenValue = tokenType, tokenValue
		









