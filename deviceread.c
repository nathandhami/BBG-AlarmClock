/*
 * deviceread.c
 *
 *  Created on: Oct 27, 2017
 *      Author: ndhami
 */

#include <pthread.h>
#include <stdbool.h>
#include "keypad.h"

static pthread_t deviceReadThreadId;
static void *readingInputs(void *arg);

static _Bool isReading = true;

void DeviceRead_startReading(void)
{
	pthread_create(&deviceReadThreadId, NULL, readingInputs, NULL);
}

static void *readingInputs(void *arg)
{
	while (isReading)
	{

	}
}

void DeviceRead_stopReading(void)
{
	isReading = false;
}
