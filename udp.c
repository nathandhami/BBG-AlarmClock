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
#include "sorter.h"

#define MSG_MAX_LEN 4096 * 3
#define PORT		12345

void UDP_startServer(void);
void UDP_stopServer(void);
_Bool UDP_checkStop(void);


static void *server(void *args);

static pthread_t id;
static int socketDescriptor;
static _Bool stopProgram = false;
static int * array;

void UDP_startServer(void)
{
	pthread_create(&id, NULL, &server, NULL);
}

void UDP_stopServer(void)
{
	if(array != NULL) {
		free(array);
	}
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
		bool dataNotSent = true;
		char* token = strtok(message, " ");
		if((strcmp(message, "help\n") == 0) || (strcmp(message, "help") == 0)) {
			strcpy(message, "Accepted command examples:\ncount -- display number arrays sorted.\nget length -- display length of array currently being sorted.\nget array -- display the full array being sorted.\nget 10 -- display the tenth element of array currently being sorted.\nstop -- cause the server program to end.\n");

		} else if(strcmp(token, "count\n") == 0) {
			long long count = Sorter_getNumberArraysSorted();
			//use of snprint() learnt from user jfs @ https://stackoverflow.com/questions/2709713/how-to-convert-unsigned-long-to-string
			const int n = snprintf(NULL, 0, "%llu", count);
			if (n < MSG_MAX_LEN ) {
				snprintf(message, MSG_MAX_LEN, "%llu\n", count);
			} else {
				strcpy(message, "Count larger than max message size.");
			}

		} else if((strcmp(token, "get\n") == 0) || (strcmp(token, "get") == 0)) {
			token = strtok(NULL, " ");
			message[0] = '\0';
			if(token == NULL) {
				strcpy(message, "get requires a second argument\n");
			} else if (strcmp(token, "length\n") == 0) {
				snprintf(message, MSG_MAX_LEN, "%d\n", Sorter_getArrayLength());

			} else if (strcmp(token, "array\n") == 0) {

			} else {
				char * pEnd;
				int base = 10;
				long int enteredNum = strtol (token,&pEnd,base);
				int value = Sorter_getArrayDataAt(enteredNum);
				if(value == -1) {
					snprintf(message, MSG_MAX_LEN, "Please enter number between 1 and %d\n", Sorter_getArrayLength());
				} else {
					snprintf(message, MSG_MAX_LEN, "%d\n", value);
				}
			}

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