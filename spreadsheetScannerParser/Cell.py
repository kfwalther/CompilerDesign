'''

@author: Kevin Walther
@class: Compiler Design 605.415 Spring 2018
@assignment: Homework 2
@brief: This class defines a Cell.

'''

from enum import Enum

class CellType(Enum):
	EQU = 1
	NUM = 2
	TEXT = 3
	EMPTY = 4
	
class Cell():
	def __init__(self, cellType=CellType.EMPTY, cellValue='', controllerList=[]):
		self.cellType, self.cellValue, self.controllerList, self.userList = cellType, cellValue, controllerList, []
		
	# A method to get the cell value.
	def getCellValue(self):
		return self.cellValue
	
	





