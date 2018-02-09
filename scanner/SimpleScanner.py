'''

@author: Kevin Walther
@class: Compiler Design 605.415 Spring 2018
@assignment: Homework 1
@brief: This Python program implements a simple scanner.

'''


# from enum import Enum
import os
import sys

# Define the global lists of characters or lexemes to aid the scanning process.
keywordList = ('if', 'then', 'else', 'end', 'return')
symbolList = ('(', ')', '=', '+', ';')
whitespaceList = (' ', '\t', '\n', '\r', '\r\n')

# Define a class to hold and track the stateful scanner variables.
class StatefulScanner():
	def __init__(self):
		self.tempText = ''
		self.tempNumber = ''
		self.tokenList = []

	# Process each character from the input file individually.	
	def processChar(self, curChar):
		# Process text character.
		if curChar.isalpha():
			self.tempText += curChar.lower()
		# Process numeric character.
		if curChar.isdigit():
			self.tempNumber += curChar
		# Process symbol character.
		if curChar in symbolList:
			# Check for a completed word/number.
			self.saveMultiCharacterGroup()
			self.tokenList.append((curChar, 'SYM'))
		# Process whitespace character.
		if curChar in whitespaceList:
			# Check for a completed word/number.
			self.saveMultiCharacterGroup()
			self.tokenList.append(((curChar), 'WS'))
		
	# Save a number to the token list.
	def saveNumber(self):
		if self.tempNumber:
			self.tokenList.append((int(self.tempNumber), 'NUM'))
			self.tempNumber = ''
			
	# Save a text group as a keyword or ID to the token list.
	def saveWord(self):
		if self.tempText:
			if self.tempText in keywordList:
				self.tokenList.append((self.tempText, 'KW'))
			else:
				self.tokenList.append((self.tempText, 'ID'))
			self.tempText = ''
			
	# Save a multi-character group (words or numbers).
	def saveMultiCharacterGroup(self):
		self.saveWord()
		self.saveNumber()
	
	# Define a method to print the token list to output.
	def printTokenList(self, outputFilePath):
		fileId = open(outputFilePath, 'w')
		sys.stdout = fileId
		tokenCount = 0
		for token in self.tokenList:
			if token[1] == 'WS' and token[0] is not ' ':
				sys.stdout.write('("' + (repr(str(token[0]))).replace("'", "") + '",' + token[1] + ')')
			else:
				sys.stdout.write('("' + str(token[0]) + '",' + token[1] + ')')
			tokenCount += 1
			if token != self.tokenList[-1]:
				sys.stdout.write(',')
			if tokenCount > 5:
				print()
				tokenCount = 0
		print()
		fileId.close()

# Open the input file for reading.
def processInputFile(inputFile, outputFile):
	inputFilePath = os.path.join(os.path.dirname(os.path.abspath(__file__)), inputFile)
	outputFilePath = os.path.join(os.path.dirname(os.path.abspath(__file__)), outputFile)
	with open(inputFilePath, 'r') as fileId:
		scanner = StatefulScanner()
		for line in fileId:
			for curChar in line:
				scanner.processChar(curChar)
		scanner.printTokenList(outputFilePath)
		
# Begin code execution here.
if __name__ == "__main__":
	processInputFile('HW1-Input.txt', 'output.txt')
	
		
	
	
	
	
	
	
	
	
	
