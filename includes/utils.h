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
void Utils_changePinDirection(const char *gpioFileName, const char *dir);
void Utils_writeToOutputPin(const char *gpioFileName, int value);
_Bool Utils_readInputPin(const char *gpioFileName);
void Utils_loadVirtualCape(const char *capeFileName);

#endif /* UTILS_H_ */
