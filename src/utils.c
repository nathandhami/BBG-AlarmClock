#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

#define EXPORT_FILE "/sys/class/gpio/export"
#define CAPE_MANAGER_FILE "/sys/devices/platform/bone_capemgr/slots"

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

void Utils_changePinDirection(const char *gpioFileName, const char *dir)
{
	FILE *pFile = fopen(gpioFileName, "w");
	if (pFile == NULL){
		printf("ERROR: could not change direction %s\n", gpioFileName);
		exit(1);
	}

	fprintf(pFile, "%s", dir);
	fclose(pFile);
}

void Utils_writeToOutputPin(const char *gpioFileName, int value)
{
	FILE *pFile = fopen(gpioFileName, "w");

	if (pFile == NULL){
		printf("ERROR: could not write to file %s\n", gpioFileName);
		exit(1);
	}

	fprintf(pFile, "%d", value);
	fclose(pFile);
}

_Bool Utils_readInputPin(const char *gpioFileName)
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

void Utils_loadVirtualCape(const char *capeFileName)
{
	FILE *virtualCapeFile = fopen(CAPE_MANAGER_FILE, "w");
	if (virtualCapeFile == NULL) {
		printf("ERROR: cannot open virtual cape %s.\n", CAPE_MANAGER_FILE);
	}

	int charWritten = fprintf(virtualCapeFile, "%s", capeFileName);
	if (charWritten <= 0) {
		printf("ERROR: cannot write to virtual cape\n");
	}

	fclose(virtualCapeFile);
}

