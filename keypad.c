/*
 * keypad.c
 *
 *  Created on: Oct 27, 2017
 *      Author: ndhami
 */

#include "keypad.h"
#include "utils.h"

#define GPIO_KEYPAD_COL1 71
#define GPIO_KEYPAD_COL2 73
#define GPIO_KEYPAD_COL3 75
#define GPIO_KEYPAD_COL4 77
#define GPIO_KEYPAD_ROW1 79
#define GPIO_KEYPAD_ROW2 80
#define GPIO_KEYPAD_ROW3 81
#define GPIO_KEYPAD_ROW4 11

const int numOfRows = 4;
const int numOfCols = 4;

char keypadMap[numOfRows][numOfCols] = { { '1', '2', '3', 'A' },
										 { '4', '5', '6', 'B' },
										 { '7', '8', '9', 'C' },
										 { '*', '0', '#', 'D' }};


void Keypad_init(void)
{
	// Export GPIO PINS
	Utils_writeToExportFile(GPIO_KEYPAD_COL1);
	Utils_writeToExportFile(GPIO_KEYPAD_COL2);
	Utils_writeToExportFile(GPIO_KEYPAD_COL3);
	Utils_writeToExportFile(GPIO_KEYPAD_COL4);
	Utils_writeToExportFile(GPIO_KEYPAD_ROW1);
	Utils_writeToExportFile(GPIO_KEYPAD_ROW2);
	Utils_writeToExportFile(GPIO_KEYPAD_ROW3);
	Utils_writeToExportFile(GPIO_KEYPAD_ROW4);
}


