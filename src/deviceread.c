/*
 * deviceread.c
 *
 *  Created on: Oct 27, 2017
 *      Author: ndhami
 */

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "keypad.h"
#include "deviceread.h"
#include <unistd.h>

static pthread_t deviceReadThreadId;
static void *readingInputs(void *arg);

static _Bool isReading = true;

#define MICROSEC_PER_MILLISEC 1000
#define DEBOUNCER_TIME MICROSEC_PER_MILLISEC * 400

void DeviceRead_startReading(void)
{
	Keypad_init();
	pthread_create(&deviceReadThreadId, NULL, readingInputs, NULL);
}

static void *readingInputs(void *arg)
{
	int currentColumn = KEYPAD_ACTIVATE_COL1;
	while (isReading)
	{
//		// Algorithm for reading keypad inputs
		// Set column i
		// scan each row for column row
		// i++

		Keypad_setColumnActive(currentColumn, 1);
		_Bool isRowActive = false;

//		printf("current column %d\n", currentColumn);

		isRowActive = Keypad_readRow(KEYPAD_READ_ROW1);

		if (isRowActive == true) {
			printf("user pressed row %d, col %d: %c\n", KEYPAD_READ_ROW1, currentColumn,
					Keypad_keyPressed(KEYPAD_READ_ROW1, currentColumn));
			usleep(DEBOUNCER_TIME);
		}

		isRowActive = Keypad_readRow(KEYPAD_READ_ROW2);

		if (isRowActive == true) {
			printf("user pressed row %d, col %d: %c\n", KEYPAD_READ_ROW2,currentColumn,
					Keypad_keyPressed(KEYPAD_READ_ROW2, currentColumn));
			usleep(DEBOUNCER_TIME);
		}
		isRowActive = Keypad_readRow(KEYPAD_READ_ROW3);

		if (isRowActive == true) {
			printf("user pressed row %d, col %d: %c\n", KEYPAD_READ_ROW3, currentColumn,
					Keypad_keyPressed(KEYPAD_READ_ROW3, currentColumn));
			usleep(DEBOUNCER_TIME);
		}
		isRowActive = Keypad_readRow(KEYPAD_READ_ROW4);

		if (isRowActive == true) {
			printf("user pressed row %d, col %d: %c\n", KEYPAD_READ_ROW4, currentColumn,
					Keypad_keyPressed(KEYPAD_READ_ROW4, currentColumn));
			usleep(DEBOUNCER_TIME);
		}

		// deactivate current column
		// set next column active
		Keypad_setColumnActive(currentColumn, 0);
		currentColumn = (currentColumn + 1) % (KEYPAD_ACTIVATE_COL_MAX);

		if (currentColumn == KEYPAD_ACTIVATE_COL_MAX){
			currentColumn = 0;
		}
	}

	pthread_exit(0);
}

void DeviceRead_stopReading(void)
{
	isReading = false;
	pthread_join(deviceReadThreadId, NULL);
}
