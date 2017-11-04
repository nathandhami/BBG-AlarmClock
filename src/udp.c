#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			// for strncmp()
#include <unistd.h>			// for close()
#include <pthread.h>
#include "udp.h"
#include "alarm.h"

#define MSG_MAX_LEN 4096 * 3
#define PORT		12345

void UDP_startServer(void);
void UDP_stopServer(void);
_Bool UDP_checkStop(void);


static void *server(void *args);

static pthread_t id;
static int socketDescriptor;
static _Bool stopProgram = false;


void UDP_startServer(void)
{
	pthread_create(&id, NULL, &server, NULL);
}

void UDP_stopServer(void)
{
	close(socketDescriptor);
	pthread_join(id, NULL);
}

void *server(void *args)
{	
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;                   // Connection may be from network
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long
	sin.sin_port = htons(PORT);                 // Host to Network short

	socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

	bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

	while(!stopProgram) {
		char message[MSG_MAX_LEN];
		unsigned int sin_len = sizeof(sin);
		int bytesRx = recvfrom(socketDescriptor, message, MSG_MAX_LEN, 0, (struct sockaddr *) &sin, &sin_len);
		message[bytesRx] = 0;
		char* token = strtok(message, " ");
		if((strcmp(message, "help\n") == 0) || (strcmp(message, "help") == 0)) {
			strcpy(message, "Accepted command examples:\ncount -- display number arrays sorted.\nget length -- display length of array currently being sorted.\nget array -- display the full array being sorted.\nget 10 -- display the tenth element of array currently being sorted.\nstop -- cause the server program to end.\n");

		} else if(strcmp(token, "add") == 0) {
			char* token1 = strtok(NULL, " ");
			char* token2 = strtok(NULL, " ");
			char* pEnd;
			int base = 10;
			int hour = strtol (token1,&pEnd,base);
			int minute = strtol (token2,&pEnd,base);
			message[0] = '\0';
			printf("hour: %d    minute:%d\n", hour, minute);
			addAlarm(hour, minute);

		} else if(strcmp(token, "stop\n") == 0) {
			stopProgram = true;
			snprintf(message, MSG_MAX_LEN, "Program Terminating\n");
		} else {
			snprintf(message, MSG_MAX_LEN, "Not a valid command\n");
		}
		sin_len = sizeof(sin);
		sendto( socketDescriptor,
			message, strlen(message),
			0,
			(struct sockaddr *) &sin, sin_len);



	}
	close(socketDescriptor);
	return NULL;
}

_Bool UDP_checkStop(){
	return stopProgram;
}