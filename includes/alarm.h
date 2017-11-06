#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <alsa/asoundlib.h>

#ifndef ALARM_H
#define ALARM_H

typedef struct Alarm_t{
	int hours;
	int minutes;
	int id;
	int difficulty;
	_Bool status;
	_Bool days[7];
	bool has_beep;
}Alarm_t;

void waitDelay(long sec, long nanoSec);

void startProgram();

void checkAlarm(int hour, int minute);
void beep(bool* beep);
void resetAlarmBeep(int day);
_Bool stopAlarm();

void addAlarm(int hour, int minute);
void editAlarm(int hour, int minute, int index);
void deleteAlarm(int index);
void getAlarm();

#endif
