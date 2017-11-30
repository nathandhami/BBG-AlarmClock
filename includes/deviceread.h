/*
 * deviceread.h
 * Reads external devices connected to beaglebone in a separate thread
 */

#ifndef DEVICEREAD_H_
#define DEVICEREAD_H_


void DeviceRead_startReading(void);
void DeviceRead_stopReading(void);
char getPressed();

#endif /* DEVICEREAD_H_ */
