#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <alsa/asoundlib.h>

#ifndef ALARM_H
#define ALARM_H

#define ALARM_SIZE 20

typedef struct Alarm_t{
	int hours;
	int minutes;
	_Bool has_beep;
}Alarm_t;

Alarm_t alarm_clock[ALARM_SIZE];
int size;
int today;

void waitDelay(long sec, long nanoSec);

void startProgram();

void checkAlarm(int hour, int minute);
void beep();
void resetAlarmBeep(int day);
_Bool stopAlarm();

void addAlarm(int hour, int minute);
void editAlarm(int hour, int minute, int index);
void deleteAlarm(int index);
void getAlarm();

#endif
