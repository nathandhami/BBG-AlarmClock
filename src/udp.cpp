
#include "udp.h"

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			// for strncmp()
#include <unistd.h>			// for close()
#include <pthread.h>
#include <stdbool.h>

#define UDP_PORT 12345
#define MAX_RECEIVE_MESSAGE_LENGTH 1024
#define REPLY_BUFFER_SIZE (1500)
#define VALUES_PER_LINE 4

// THESE ARE THE IDS FOR NODEJS PACKETS.
// SETTER COMMANDS
#define COMMAND_TEST        "test"


// This macro will retrieve the data from the UDP packet
// The data being sent from webserver is assumed to be in this format: command:(Value)
#define RETRIEVE_PACKET_DATA(buffer) strchr((buffer), ':') + 1

static pthread_t s_threadId;
static char replyBuffer[REPLY_BUFFER_SIZE];
static _Bool continueUdpServer = true;

// Header
static void *udpListeningThread(void *args);
static void processUDPCommand(char* udpCommand, int socketDescriptor, struct sockaddr_in *pSin);
static int secondWordToInt(char *string);
static void concatValuesToString(char *targetBuffer, int data[], int indexStart, int indexEnd);
static char *extractPacketData(char *buffer);

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
		printf("Message received (%d bytes): \n\n'%s'\n", bytesRx, message);

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

	char *data;

	if (isUdpThisCommand(udpCommand, COMMAND_TEST)) {
		data = extractPacketData(udpCommand);
		printf("----COMMAND RECIEVED: %s----\n", data);
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
