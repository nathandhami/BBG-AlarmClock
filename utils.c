/*
 * utils.c
 *
 *  Created on: Oct 27, 2017
 *      Author: ndhami
 */

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

#define EXPORT_FILE "/sys/class/gpio/export"

#define MAX_BUFFER_SIZE 1024

void Utils_writeToExportFile(int pinNumber)
{
	FILE *pFile = fopen(EXPORT_FILE, "w");

	if (pFile == NULL){
		printf("ERROR: could not open %s\n", EXPORT_FILE);
		exit(1);
	}

	fprintf(pFile, "%d", pinNumber);
	fclose(pFile);
}

void Utils_changePinDirection(char *gpioFileName, char *dir)
{
	FILE *pFile = fopen(gpioFileName, "w");
	if (pFile == NULL){
		printf("ERROR: could not change direction %s\n", gpioFileName);
		exit(1);
	}

	fprintf(pFile, "%s", dir);
	fclose(pFile);
}

void Utils_writeToOutputPin(char *gpioFileName, int value)
{
	FILE *pFile = fopen(gpioFileName, "w");

	if (pFile == NULL){
		printf("ERROR: could not write to file %s\n", gpioFileName);
		exit(1);
	}

	fprintf(pFile, "%d", value);
	fclose(pFile);
}

_Bool Utils_readInputPin(char *gpioFileName)
{
	FILE *pFile = fopen(gpioFileName, "r");
	if (pFile == NULL){
		printf("ERROR: could not read input file  %s\n", gpioFileName);
		exit(1);
	}

	char buffer[MAX_BUFFER_SIZE];

	fgets(buffer, MAX_BUFFER_SIZE, pFile);

	fclose(pFile);

	int val = atoi(buffer);

	return val == 1 ? true:false;
}
