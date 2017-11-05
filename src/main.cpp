/**
 ** i2c LCD test application
 **
 ** Author: Kaj-Michael Lang <milang@tal.org>
 ** Copyright 2014 - Under creative commons license 3.0 Attribution-ShareAlike CC BY-SA
 **/
#include "LiquidCrystal_I2C.h"
#include "utils.h"
#include "udp.h"
#include <stdio.h>
#include <ctime>
#include <chrono>
#include <iostream>

using namespace std;
#define VIRTUAL_CAPE_FILE "/sys/devices/platform/bone_capemgr/slots"


void* timeThread(void);

bool threadDone = 0;

int main (int argc, char *argv []) {

	// start upd thread to connect with nodejs webserver
	UdpListener_startListening();

	//set virtual cape
	Utils_loadVirtualCape("BB-I2C1");

	// i2c address
	uint8_t i2c=0x3f;
	// Control line PINs
	uint8_t en=2;
	uint8_t rw=1;
	uint8_t rs=0;

	// Data line PINs
	uint8_t d4=4;
	uint8_t d5=5;
	uint8_t d6=6;
	uint8_t d7=7;

	// Backlight PIN
	uint8_t bl=3;

	// LCD display size
	uint8_t rows=2;
	uint8_t cols=16;

	LiquidCrystal_I2C lcd("/dev/i2c-1", i2c, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);

	lcd.begin(cols, rows);

	lcd.on();
	lcd.clear();

	struct timespec waitLong;
	waitLong.tv_nsec = 0;
	waitLong.tv_sec = 1;

	while(true) {
		lcd.clear();

		auto now = std::chrono::system_clock::now();
		time_t t = chrono::system_clock::to_time_t(now);
		tm local_tm = *gmtime(&t);
		int hour = (local_tm.tm_hour - 7) % 12;
		if(hour == 0) {
			hour = 12;
		}
		int minute = local_tm.tm_min;
		int second = local_tm.tm_sec;
		char buffer[16];
		sprintf (buffer, "%d:%d:%d", hour, minute, second);
		printf("%s\n", buffer);
		lcd.print(buffer);
		nanosleep(&waitLong, NULL);
	}

	// lcd.print("ABCDEFGH");
	// lcd.setCursor(0, 1);
	// lcd.print("12345678");

	// if (rows>2) {
	// 	lcd.setCursor(0, 2);
	// 	lcd.print("ABCDEFGH");
	// 	lcd.setCursor(0, 3);
	// 	lcd.print("12345678");
	// }

	// sleep(2);

	// lcd.clear();
	// lcd.write('H');
	// lcd.write('e');
	// lcd.write('l');
	// lcd.write('l');
	// lcd.write('o');
	// lcd.setCursor(0, 1);
	// lcd.write('W');
	// lcd.write('o');
	// lcd.write('r');
	// lcd.write('l');
	// lcd.write('d');
	// lcd.write('!');

	// if (rows>2) {
	// 	lcd.setCursor(0,2);
	// 	lcd.print("We say");
	// 	lcd.setCursor(0,3);
	// 	lcd.print("H'ello back!");
	// }

	// sleep(2);
	// lcd.blink();
	// sleep(2);

	// lcd.clear();
	// lcd.cursor();
	// lcd.autoscroll();
	// for (uint8_t i=33; i<255; i++) {
	// 	usleep(35000);
	// 	lcd.write((char)i);
	// 	if (i % 8==0)
	// 		sleep(1);
	// }
	// sleep(1);
	// lcd.noBlink();
	// lcd.noCursor();
	// sleep(1);
	// lcd.clear();
}

// void* timeThread(){
// 	_Bool initialize_today = false;
// 	while(threadDone == 0){
// 		time_t t;
// 		time(&t);
// 		char* temp_str = ctime(&t);
// 		char* hour = (char*) malloc(sizeof(char)*3);
// 		hour[0] = temp_str[11];
// 		hour[1] = temp_str[12];
// 		hour[2] = '\0';
// 		char* minute = (char*) malloc(sizeof(char)*3);
// 		minute[0] = temp_str[14];
// 		minute[1] = temp_str[15];
// 		minute[2] = '\0';
// 		char* day = (char*) malloc(sizeof(char)*3);
// 		day[0] = temp_str[8];
// 		day[1] = temp_str[9];
// 		day[2] = '\0';
// 		if(initialize_today == false){
// 			today = atoi(day);
// 			initialize_today = true;
// 		}
// 		checkAlarm(atoi(hour),atoi(minute));
// 		resetAlarmBeep(atoi(day));
// 		free(hour);
// 		free(minute);
// 		free(day);
// 	}
// 	return NULL;
// }
