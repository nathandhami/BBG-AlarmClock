/*
 * utils.h
 *
 *  Created on: Oct 27, 2017
 *      Author: ndhami
 *      Purpose: This module will contain helper functions.
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdbool.h>

void Utils_writeToExportFile(int pinNumber);
void Utils_changePinDirection(char *gpioFileName, char *dir);
void Utils_writeToOutputPin(char *gpioFileName, int value);
_Bool Utils_readInputPin(char *gpioFileName);


#endif /* UTILS_H_ */
