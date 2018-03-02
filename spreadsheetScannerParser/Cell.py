'''

@author: Kevin Walther
@class: Compiler Design 605.415 Spring 2018
@assignment: Homework 2
@brief: This class defines a Cell.

'''

from enum import Enum

# Define the types of Cell we can create.
class CellType(Enum):
	EQU = 1
	NUM = 2
	TEXT = 3
	EMPTY = 4

# Define the types of values that can be stored in Cells.
class CellValueType(Enum):
	INT = 1
	FLOAT = 2
		
class Cell():
	def __init__(self, cellType=CellType.EMPTY, cellValue='', cellValueType=None, parseTree=None, controllerList=[]):
		self.cellType, self.cellValue, self.cellValueType, self.parseTree, self.controllerList, self.userList = cellType, cellValue, cellValueType, None, controllerList, []
		
	# A method to get the cell value.
	def getCellValue(self):
		return self.cellValue
	
	





