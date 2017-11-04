/**
 ** i2c LCD test application
 **
 ** Author: Kaj-Michael Lang <milang@tal.org>
 ** Copyright 2014 - Under creative commons license 3.0 Attribution-ShareAlike CC BY-SA
 **/
extern "C" {
	#include "alarm.h"
}

#include "LiquidCrystal_I2C.h"
#include "utils.h"
#include <stdio.h>
#include <ctime>
#include <chrono>
#include <iostream>
using namespace std;
#define VIRTUAL_CAPE_FILE "/sys/devices/platform/bone_capemgr/slots"


void* timeThread(void);

bool threadDone = 0;

int main (int argc, char *argv []) {

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
	startProgram();

	LiquidCrystal_I2C lcd("/dev/i2c-1", i2c, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);

	lcd.begin(cols, rows);

	lcd.on();
	lcd.clear();

	struct timespec waitLong;
	waitLong.tv_nsec = 0;
	waitLong.tv_sec = 1;

	setenv("TZ", "PST8PST", 1);   // set TZ
	tzset();

	bool initialize_today = false;

	while(true) {

		lcd.clear();

		auto now = std::chrono::system_clock::now();
		time_t t = chrono::system_clock::to_time_t(now);
		tm local_tm = *localtime(&t);
		int hour = local_tm.tm_hour; //- 7) % 12;
		// if(hour == 0) {
		// 	hour = 12;
		// }
		int day = local_tm.tm_wday;
		int minute = local_tm.tm_min;
		int second = local_tm.tm_sec;
		if(initialize_today == false){
			today = day;
			initialize_today = true;
		}
		checkAlarm(hour, minute);
		resetAlarmBeep(day);


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
