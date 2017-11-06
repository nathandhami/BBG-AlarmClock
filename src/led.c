/*
 * led.c
 *
 *  Created on: Nov 6, 2017
 *      Author: ndhami
 */

#include "utils.h"
#include "led.h"

#define LED_GPIO_PIN 68 // pin 10 on beagle bone
#define LED_GPIO_FILEPATH_VAL "/sys/class/gpio/gpio68/value"
#define LED_GPIO_FILEPATH_DIR "/sys/class/gpio/gpio68/direction"

static int status = 0;


void LED_init(void)
{
	Utils_writeToExportFile(LED_GPIO_PIN);
	Utils_changePinDirection(LED_GPIO_FILEPATH_DIR, "out");
	LED_deactivate();
}


void LED_activate(void)
{
	Utils_writeToOutputPin(LED_GPIO_FILEPATH_VAL, 1);
	status = 1;
}

void LED_deactivate(void)
{
	Utils_writeToOutputPin(LED_GPIO_FILEPATH_VAL, 0);
	status = 0;
}


int LED_getStatus(void)
{
	return status;
}
