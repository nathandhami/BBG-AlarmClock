/*
 * keypad.c
 *
 *  Created on: Oct 27, 2017
 *      Author: ndhami
 */

#include "keypad.h"
#include "utils.h"
#include <stdio.h>

// GPIO Pins setup synchronized with hardware setup (KEYPAD Matrix 4x4)
// COLUMNS 1 - 4 are output pins
// ROWS 1 -4 are input pins
#define GPIO_KEYPAD_COL1 71
#define GPIO_KEYPAD_COL2 73
#define GPIO_KEYPAD_COL3 75
#define GPIO_KEYPAD_COL4 77
#define GPIO_KEYPAD_ROW1 79
#define GPIO_KEYPAD_ROW2 80
#define GPIO_KEYPAD_ROW3 81
#define GPIO_KEYPAD_ROW4 11

#define GPIO_KEYPAD_COL1_FILEPATH_DIR "/sys/class/gpio/gpio71/direction"
#define GPIO_KEYPAD_COL2_FILEPATH_DIR "/sys/class/gpio/gpio73/direction"
#define GPIO_KEYPAD_COL3_FILEPATH_DIR "/sys/class/gpio/gpio75/direction"
#define GPIO_KEYPAD_COL4_FILEPATH_DIR "/sys/class/gpio/gpio77/direction"

#define GPIO_KEYPAD_COL1_FILEPATH_VAL "/sys/class/gpio/gpio71/value"
#define GPIO_KEYPAD_COL2_FILEPATH_VAL "/sys/class/gpio/gpio73/value"
#define GPIO_KEYPAD_COL3_FILEPATH_VAL "/sys/class/gpio/gpio75/value"
#define GPIO_KEYPAD_COL4_FILEPATH_VAL "/sys/class/gpio/gpio77/value"
#define GPIO_KEYPAD_ROW1_FILEPATH_VAL "/sys/class/gpio/gpio79/value"
#define GPIO_KEYPAD_ROW2_FILEPATH_VAL "/sys/class/gpio/gpio80/value"
#define GPIO_KEYPAD_ROW3_FILEPATH_VAL "/sys/class/gpio/gpio81/value"
#define GPIO_KEYPAD_ROW4_FILEPATH_VAL "/sys/class/gpio/gpio11/value"


#define NUM_OF_ROWS 4
#define NUM_OF_COLS 4

// Row 1, column 1 starts at the bottom right for the keypad hardware
const static char keypadMap[NUM_OF_ROWS][NUM_OF_COLS] = { { 'D', '#', '0', '*' },
										 	 	 	 	  { 'C', '9', '8', '7' },
														  { 'B', '6', '5', '4' },
														  { 'A', '3', '2', '1' }};


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

	// Configure pins
	Utils_changePinDirection(GPIO_KEYPAD_COL1_FILEPATH_DIR, "out");
	Utils_changePinDirection(GPIO_KEYPAD_COL2_FILEPATH_DIR, "out");
	Utils_changePinDirection(GPIO_KEYPAD_COL3_FILEPATH_DIR, "out");
	Utils_changePinDirection(GPIO_KEYPAD_COL3_FILEPATH_DIR, "out");
}

void Keypad_setColumnActive(eKEYPAD_ACTIVATE_COL col)
{
	switch (col){
		case KEYPAD_ACTIVATE_COL1:
			Utils_writeToOutputPin(GPIO_KEYPAD_COL1_FILEPATH_VAL, 1);
			break;
		case KEYPAD_ACTIVATE_COL2:
			Utils_writeToOutputPin(GPIO_KEYPAD_COL2_FILEPATH_VAL, 1);
			break;
		case KEYPAD_ACTIVATE_COL3:
			Utils_writeToOutputPin(GPIO_KEYPAD_COL3_FILEPATH_VAL, 1);
			break;
		case KEYPAD_ACTIVATE_COL4:
			Utils_writeToOutputPin(GPIO_KEYPAD_COL4_FILEPATH_VAL, 1);
			break;
		default:
			printf("Unable to set column active\n");
			break;
	}
}

_Bool Keypad_readRow(eKEYPAD_READ_ROW row)
{
	_Bool rowActive = false;

	switch (row){
		case KEYPAD_READ_ROW1:
			rowActive = Utils_readInputPin(GPIO_KEYPAD_ROW1_FILEPATH_VAL);
			break;
		case KEYPAD_READ_ROW2:
			rowActive = Utils_readInputPin(GPIO_KEYPAD_ROW2_FILEPATH_VAL);
			break;
		case KEYPAD_READ_ROW3:
			rowActive = Utils_readInputPin(GPIO_KEYPAD_ROW3_FILEPATH_VAL);
			break;
		case KEYPAD_READ_ROW4:
			rowActive = Utils_readInputPin(GPIO_KEYPAD_ROW4_FILEPATH_VAL);
			break;
		default:
			printf("Unable to read row\n");
			break;
	}

	return rowActive;
}

char Keypad_keyPressed(eKEYPAD_READ_ROW row, eKEYPAD_ACTIVATE_COL col)
{
	return keypadMap[row][col];
}
