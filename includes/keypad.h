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
	KEYPAD_COL_1 = 0,
	KEYPAD_COL_2,
	KEYPAD_COL_3,
	KEYPAD_COL_4,
	KEYPAD_COL_MAX = 4,
}eKEYPAD_COL;

typedef enum {
	KEYPAD_ROW_1 = 0,
	KEYPAD_ROW_2,
	KEYPAD_ROW_3,
	KEYPAD_ROW_4,
	KEYPAD_ROW_MAX = 4,
}eKEYPAD_ROW;

void Keypad_init(void);

void Keypad_setColumnActive(eKEYPAD_COL col, int value);

_Bool Keypad_readRow(eKEYPAD_ROW row);

char Keypad_keyPressed( eKEYPAD_ROW row, eKEYPAD_COL col);

#endif /* KEYPAD_H_ */
