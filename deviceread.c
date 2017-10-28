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

void DeviceRead_startReading(void)
{
	Keypad_init();
	pthread_create(&deviceReadThreadId, NULL, readingInputs, NULL);
}

static void *readingInputs(void *arg)
{
	int sum = 0;
	while (isReading)
	{
//		// Algorithm for reading keypad inputs
		// Set column i
		// scan each row for column row
		// i++
		Keypad_setColumnActive(KEYPAD_ACTIVATE_COL1);
		_Bool isRowActive = false;

//		for (int i = 0; i < KEYPAD_READ_ROW_MAX; i++){

			isRowActive = Keypad_readRow(KEYPAD_READ_ROW1);
//		}
		if (isRowActive == true){
			sum++;
			printf("user pressed %d: %c\n",sum, Keypad_keyPressed(KEYPAD_READ_ROW1, KEYPAD_ACTIVATE_COL1));
			usleep(200000);
		}
	}

	pthread_exit(0);
}

void DeviceRead_stopReading(void)
{
	isReading = false;
	pthread_join(deviceReadThreadId, NULL);
}
