'''

@author: Kevin Walther
@class: Compiler Design 605.415 Spring 2018
@assignment: Homework 5
@brief: This class defines the Spreadsheet data structure for storing the Cells.

'''

import Cell


indexToColDict = {
    1: 'A',
    2: 'B',       
    3: 'C',
    4: 'D',
    5: 'E',
    6: 'F'       
}

class Spreadsheet():
    def __init__(self):
        self.spreadsheetDict = {}
        for row in range(0,10):
            for col in range(1,7):
                self.spreadsheetDict[(indexToColDict[col] + str(row))] = Cell.Cell()
                        

    # Define a method to retrieve the contents of a particular cell.
    def getCell(self, cellLocation):
        return self.spreadsheetDict[self.verifyId(cellLocation)]
    
    # Define a method to clear the contents of a particular cell.
    def clearCell(self, cellLocation):
        self.spreadsheetDict[self.verifyId(cellLocation)] = Cell.Cell()
        
    # Write a function to validate the Cell position names (e.g. A0, C4, etc.)
    def verifyId(self, tempId):
        if len(tempId) != 2:
            raise ValueError('Malformed ID value detected: ' + tempId)
        col, row = tempId
        if (not col.isalpha()) or (not row.isnumeric()):
            raise ValueError('Malformed ID value detected:' + tempId)
        if (ord(col) not in range(ord('A'), ord('G'))):
            raise ValueError('Invalid column reference in ID (columns must be within range A to F):' + col)
        if (int(row) not in range(0, 10)):
            raise ValueError('Invalid row reference in ID (rows must be within range 0 to 9):' + row)
        return tempId

    # Write a function to validate the token ID names (e.g. A0, C4, etc.)
    def isId(self, tempId):
        if len(tempId) != 2:
            return 'other'
        col, row = tempId
        if (not col.isalpha()) or (not row.isnumeric()):
            return 'other'
        if (ord(col) not in range(ord('A'), ord('G'))):
            return 'other'
        if (int(row) not in range(0, 10)):
            return 'other'
        return 'id'
    
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

    # Define a method to print an individual cell of data.
    def printCell(self, cellContents):
        return str(cellContents) + (' ' * (8 - len(str(cellContents)))) + '|'

    # Define a method to print out the parsed tokens.
    def printSpreadsheet(self):
        # Print out the grid, or spreadsheet.
        print('|' + self.printCell('') + self.printCell('A') + self.printCell('B') + 
                self.printCell('C') + self.printCell('D') + self.printCell('E') + self.printCell('F'))
        for row in range(0,10):
            tempStr = '|' + self.printCell(row)
            for col in range(1,7):
                tempStr = tempStr + self.printCell(self.spreadsheetDict.get((indexToColDict[col] + str(row)), '').cellValue)
            print(tempStr)


                    






    