/*
 * joystick.c
 *
 *  Created on: Nov 11, 2017
 *      Author: ndhami
 */

#include "joystick.h"
#include "utils.h"

#define JOYSTICK_UP "/sys/class/gpio/gpio26/value"
#define JOYSTICK_DOWN "/sys/class/gpio/gpio46/value"

#define JOYSTICK_UP_GPIO 26
#define JOYSTICK_DOWN_GPIO 46


void Joystick_init(void)
{
	Utils_writeToExportFile(JOYSTICK_UP_GPIO);
	Utils_writeToExportFile(JOYSTICK_DOWN_GPIO);
}

_Bool Joystick_checkUp(void)
{
	// readInputPin returns 0 if pressed
	return !Utils_readInputPin(JOYSTICK_UP);
}

_Bool Joystick_checkDown(void)
{
	return !Utils_readInputPin(JOYSTICK_DOWN);
}
