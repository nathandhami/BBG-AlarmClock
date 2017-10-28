/*
 * utils.c
 *
 *  Created on: Oct 27, 2017
 *      Author: ndhami
 */

#include "utils.h"
#include <stdio.h>

#define EXPORT_FILE "/sys/class/gpio/export"

void writeToExportFile(int pinNumber)
{
	FILE *pFile = fopen(EXPORT_FILE, "w");

	if (pFile == NULL){
		printf("ERROR: could not open %s\n", EXPORT_FILE);
		exit(1);
	}

	fprintf(pFile, "%d", pinNumber);
	fclose(pFile);
}
