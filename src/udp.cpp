
#include "udp.h"
#include "alarm.h"

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			// for strncmp()
#include <unistd.h>			// for close()
#include <pthread.h>
#include <stdbool.h>
#include <iostream>
#include <vector>
#include <string.h>

#define UDP_PORT 12345
#define MAX_RECEIVE_MESSAGE_LENGTH 8000
#define REPLY_BUFFER_SIZE (1500)
#define VALUES_PER_LINE 4
#define DAYS_IN_WEEK 7

// THESE ARE THE IDS FOR NODEJS PACKETS.
// SETTER COMMANDS
#define COMMAND_INIT_ALARMS		"initArray"
#define COMMAND_CREATE_ALARM	"createAlarm"
#define COMMAND_EDIT_ALARM		"editAlarm"
#define COMMAND_DELETE_ALARM	"deleteAlarm"
#define COMMAND_CHANGE_SOUND    "changeSound"


// This macro will retrieve the data from the UDP packet
// The data being sent from webserver is assumed to be in this format: command:(Value)
#define RETRIEVE_PACKET_DATA(buffer) strchr((buffer), ':') + 1

using namespace std;

static pthread_t s_threadId;
static char replyBuffer[REPLY_BUFFER_SIZE];
static _Bool continueUdpServer = true;

// Header
static void *udpListeningThread(void *args);
static void processUDPCommand(char* udpCommand, int socketDescriptor, struct sockaddr_in *pSin);
static char *extractPacketData(char *buffer);
static vector<char*> splitString(char* str, char* delimiter);
static vector<Alarm_t> parseAlarmData(char* alarmData);

void UDP_startServer(void)
{
	printf("Starting UDP server...\n");
	pthread_create(&s_threadId, NULL, &udpListeningThread, NULL);
}

void UdpListener_cleanup(void)
{
	continueUdpServer = false;
	pthread_join(s_threadId, NULL);
}



static void *udpListeningThread(void *args)
{
	// Buffer to hold packet data:
	char message[MAX_RECEIVE_MESSAGE_LENGTH];

	// Address
	struct sockaddr_in sin;
	unsigned int sin_len;						// May change.
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);	// Host to Network long
	sin.sin_port = htons(UDP_PORT);				// Host to Network short

	// Create the socket for UDP
	int socket_descriptor = socket(PF_INET, SOCK_DGRAM, 0);

	// Bind the socket to the port that we specify
	bind(socket_descriptor, (struct sockaddr*) &sin, sizeof(sin));

	while (continueUdpServer == true) {
		// Get the data (blocking)
		// Will change sin (the address) to be the address of the client.
		sin_len = sizeof(sin);
		int bytesRx = recvfrom(socket_descriptor, message, MAX_RECEIVE_MESSAGE_LENGTH, 0,
				(struct sockaddr *) &sin, &sin_len);

		// Make it null terminated (so string functions work):
		message[bytesRx] = 0;
		printf("Message received (%d bytes): \n%s\n\n", bytesRx, message);

		processUDPCommand(message, socket_descriptor, &sin);

		// Transmit a reply (if desired):
		if (strnlen(replyBuffer, REPLY_BUFFER_SIZE) > 0) {
			sin_len = sizeof(sin);
			sendto( socket_descriptor,
				replyBuffer, strnlen(replyBuffer, REPLY_BUFFER_SIZE),
				0,
				(struct sockaddr *) &sin, sin_len);
		}
	}

	// Close socket on shut-down
	close(socket_descriptor);
	return NULL;
}

_Bool isUdpThisCommand(char* udpMessage, const char* command)
{
	return strncmp(udpMessage, command, strlen(command)) == 0;
}


static void processUDPCommand(char* udpCommand, int socketDescriptor, struct sockaddr_in *pSin)
{
	replyBuffer[0] = 0;

	sprintf(replyBuffer, "Recv'd on c application.\n");

	char* data;

	if (isUdpThisCommand(udpCommand, COMMAND_INIT_ALARMS)) {
		data = extractPacketData(udpCommand);
		vector<Alarm_t> alarmClocks = parseAlarmData(data);

		//INITIALISE alarm array (change array to vector) in alarm.c to above vector object
		for(int i = 0; i < alarmClocks.size(); i++){
			Alarm_addAlarm(alarmClocks[i].hours, alarmClocks[i].minutes, 					alarmClocks[i].id, alarmClocks[i].difficulty, 					alarmClocks[i].status, alarmClocks[i].days);
		}
	} 
	else if (isUdpThisCommand(udpCommand, COMMAND_CREATE_ALARM)) {
		data = extractPacketData(udpCommand);
		struct Alarm_t alarm = parseAlarmData(data)[0];

		//CREATE new alarm in alarm.c from above new alarm
		Alarm_addAlarm(alarm.hours, alarm.minutes, alarm.id, alarm.difficulty, 					alarm.status, alarm.days);
	}
	else if (isUdpThisCommand(udpCommand, COMMAND_EDIT_ALARM)) {
		data = extractPacketData(udpCommand);
		struct Alarm_t alarm = parseAlarmData(data)[0];

		//EDIT alarm in alarm.c from above object
		//use ID field in alarm[i] to edit which one
		Alarm_editAlarm(alarm.hours, alarm.minutes, alarm.id, alarm.difficulty, 				alarm.status, alarm.days);
	} 
	else if (isUdpThisCommand(udpCommand, COMMAND_DELETE_ALARM)) {
		data = extractPacketData(udpCommand);
		int id;
		if (data != NULL) {
			id = atoi(data);
		}

		//DELETE alarm using id from alarm object
		Alarm_deleteAlarm(id);
	}
	else if (isUdpThisCommand(udpCommand, COMMAND_CHANGE_SOUND)) {
		data = extractPacketData(udpCommand);
		int id;
		if (data != NULL) {
			//Change sound used for alarm
			Alarm_changeSound(data);
		}

	}
}

static char *extractPacketData(char *buffer)
{
	char *extractData = NULL;
	// Retrieve the data from the UDP packet
	// The data being sent from webserver is assumed to be in this format: command:(Value)
	if (buffer != NULL)
	{
		// Data will appear after the semicolon
		extractData = strchr(buffer, ':') + 1;
	}

	return extractData;
}

static vector<char*> splitString(char* str, char* delimiter) {
	vector<char*> packetElements;
	char* element;

	element = strtok(str, delimiter);
	while (element != NULL)
	{
		packetElements.push_back(element);
		element = strtok (NULL, delimiter);
	}

	return packetElements;
}

static vector<Alarm_t> parseAlarmData(char* alarmData) {

	vector<Alarm_t> totalAlarms;

	char* delimiter = (char *)"\n";
	vector<char*> alarms = splitString(alarmData, delimiter);

	for (unsigned int i = 0; i < alarms.size(); i++) {
		delimiter = (char *)"-";
		vector<char*> alarmSplitted = splitString(alarms[i], delimiter);

		delimiter = (char *)"=";
		char* timeString = splitString(alarmSplitted[0], delimiter)[1];
		char* statusString = splitString(alarmSplitted[1], delimiter)[1];
		char* levelString = splitString(alarmSplitted[2], delimiter)[1];
		char* daysString = splitString(alarmSplitted[3], delimiter)[1];
		char* idString = splitString(alarmSplitted[4], delimiter)[1];

		_Bool isTimePM;
		if (((string)timeString).find("PM") != string::npos) {
		    isTimePM = true;
		}

		delimiter = (char *)" ";
		char* timeNumberString = splitString(timeString, delimiter)[0];
		delimiter = (char *)":";
		int hour = atoi(splitString(timeNumberString, delimiter)[0]);
		int mins =  atoi(splitString(timeNumberString, delimiter)[1]);
		if (isTimePM) {
			if (hour < 12) {
				hour += 12;
			}
		}

		_Bool status = true;
		if (strcmp(statusString, "false") == 0) {
			status = false;
		}

		int difficulty = 0;
		if (strcmp(levelString, "medium") == 0) {
			difficulty = 1;
		} else if (strcmp(levelString, "hard") == 0) {
			difficulty = 2;
		}

		delimiter = (char *)",";
		vector<char*> isDaysOn = splitString(daysString, delimiter);
		_Bool days[] = {false, false, false, false, false, false, false};
		for (unsigned int j = 0; j < isDaysOn.size(); j++) {
			if (strcmp(isDaysOn[j], "on") == 0) {
				days[j] = true;
			}
		}

		int id = atoi(idString);

		struct Alarm_t a;
		a.hours = hour;
		a.minutes = mins;
		a.status = status;
		a.difficulty = difficulty;
		a.id = id;
		for (int j = 0; j < DAYS_IN_WEEK; j++) {
			a.days[j] = days[j];
		}

		totalAlarms.push_back(a);
	}

	return totalAlarms;

}
