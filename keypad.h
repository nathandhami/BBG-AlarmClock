/*
 * keypad.h
 *
 *  Created on: Oct 27, 2017
 *      Author: ndhami
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <stdbool.h>

typedef enum {
	KEYPAD_ACTIVATE_COL1 = 0,
	KEYPAD_ACTIVATE_COL2,
	KEYPAD_ACTIVATE_COL3,
	KEYPAD_ACTIVATE_COL4,
	KEYPAD_ACTIVATE_COL_MAX = 4,
}eKEYPAD_ACTIVATE_COL;

typedef enum {
	KEYPAD_READ_ROW1 = 0,
	KEYPAD_READ_ROW2,
	KEYPAD_READ_ROW3,
	KEYPAD_READ_ROW4,
	KEYPAD_READ_ROW_MAX = 4,
}eKEYPAD_READ_ROW;

void Keypad_init(void);

void Keypad_setColumnActive(eKEYPAD_ACTIVATE_COL col);

_Bool Keypad_readRow(eKEYPAD_READ_ROW row);

char Keypad_keyPressed( eKEYPAD_READ_ROW row, eKEYPAD_ACTIVATE_COL col);

#endif /* KEYPAD_H_ */
