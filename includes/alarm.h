// alarm.h
// Main interface for managing alarms and starts up the skill-testing alarm clock thread
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
	int questionType;
	_Bool status;
	_Bool days[7];
}Alarm_t;

void waitDelay(long sec, long nanoSec);

void Alarm_startProgram();
void Alarm_endProgram();

void Alarm_addAlarm(Alarm_t alarm);
void Alarm_editAlarm(Alarm_t alarm);
void Alarm_deleteAlarm(int index);
void Alarm_getAlarm();

void Alarm_changeSound(const char *newSound);

#endif
