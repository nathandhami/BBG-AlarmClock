#define _BSD_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "keypad.h"
#include "deviceread.h"
#include <unistd.h>

static pthread_t deviceReadThreadId;
static void *readingInputs(void *arg);

static _Bool isReading = true;
static char pressed = 'x';

#define MICROSEC_PER_MILLISEC 1000
#define DEBOUNCER_TIME MICROSEC_PER_MILLISEC * 400

void DeviceRead_startReading(void)
{
	Keypad_init();
	pthread_create(&deviceReadThreadId, NULL, readingInputs, NULL);
}

char getPressed() {
	return pressed;
}

static void *readingInputs(void *arg)
{
	int currentColumn = KEYPAD_COL_1;
	while (isReading)
	{
//		// Algorithm for reading keypad inputs
		// Set column i
		// scan each row for column row
		// i++
		bool nothingPressed = true;
		Keypad_setColumnActive(currentColumn, 1);
		_Bool isRowActive = false;

//		printf("current column %d\n", currentColumn);

		isRowActive = Keypad_readRow(KEYPAD_ROW_1);

		if (isRowActive == true) {
			pressed = Keypad_keyPressed(KEYPAD_ROW_1, currentColumn);
			nothingPressed = false;
			// printf("user pressed row %d, col %d: %c\n", KEYPAD_ROW_1, currentColumn,
			// 		pressed);
			usleep(DEBOUNCER_TIME);
		}

		isRowActive = Keypad_readRow(KEYPAD_ROW_2);

		if (isRowActive == true) {
			pressed = Keypad_keyPressed(KEYPAD_ROW_2, currentColumn);
			nothingPressed = false;
			// printf("user pressed row %d, col %d: %c\n", KEYPAD_ROW_2,currentColumn,
			// 		pressed);
			usleep(DEBOUNCER_TIME);
		}
		isRowActive = Keypad_readRow(KEYPAD_ROW_3);

		if (isRowActive == true) {
			pressed = Keypad_keyPressed(KEYPAD_ROW_3, currentColumn);
			nothingPressed = false;
			// printf("user pressed row %d, col %d: %c\n", KEYPAD_ROW_3, currentColumn,
			// 		pressed);
			usleep(DEBOUNCER_TIME);
		}
		isRowActive = Keypad_readRow(KEYPAD_ROW_4);

		if (isRowActive == true) {
			pressed = Keypad_keyPressed(KEYPAD_ROW_4, currentColumn);
			nothingPressed = false;
			// printf("user pressed row %d, col %d: %c\n", KEYPAD_ROW_4, currentColumn,
			// 		pressed);
			usleep(DEBOUNCER_TIME);
		}

		// deactivate current column
		// set next column active
		Keypad_setColumnActive(currentColumn, 0);
		currentColumn = (currentColumn + 1) % (KEYPAD_COL_MAX);

		if (currentColumn == KEYPAD_COL_MAX){
			currentColumn = 0;
		}

		if(nothingPressed) {
			pressed = ' ';
		}
	}

	pthread_exit(0);
}

void DeviceRead_stopReading(void)
{
	isReading = false;
	pthread_join(deviceReadThreadId, NULL);
}
