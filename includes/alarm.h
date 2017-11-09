#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#ifndef ALARM_H
#define ALARM_H

typedef struct Alarm_t{
	int hours;
	int minutes;
	int id;
	int difficulty;
	_Bool status;
	_Bool days[7];
}Alarm_t;

void waitDelay(long sec, long nanoSec);

void Alarm_startProgram();
void Alarm_endProgram();

void Alarm_addAlarm(int hour, int minute, int ids, _Bool stats, _Bool day[7]);
void Alarm_editAlarm(int hour, int minute, int ids, _Bool stats, _Bool day[7]);
void Alarm_deleteAlarm(int index);
void Alarm_getAlarm();

#endif
