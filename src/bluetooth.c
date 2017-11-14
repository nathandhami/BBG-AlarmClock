// Source: http://www.cs.sfu.ca/CourseCentral/433/bfraser/other/2015-student-howtos/BBBluetoothGuide.pdf

#include <sys/socket.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <pthread.h>
#include <stdbool.h>

#define SMALL_BUFFER_SIZE 256
#define RECEIVE_BUFFER_SIZE 1024

static pthread_t bluetoothThreadId;
static _Bool running = true;

static void *runBluetoothServer(void *arg);


void Bluetooth_startServer(void)
{
	pthread_create(&bluetoothThreadId, NULL, runBluetoothServer, NULL);
}


static void *runBluetoothServer(void *arg)
{
	// allocate socket
	int socketDesc = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	// binding socket to port 1 of the first available
	struct sockaddr_rc server_addr = { 0 };
	server_addr.rc_family = AF_BLUETOOTH;
	server_addr.rc_bdaddr = *BDADDR_ANY;
	server_addr.rc_channel = (uint8_t) 1;
	bind(socketDesc, (struct sockaddr *)&server_addr, sizeof(server_addr));

	// put socket into listening mode
	listen(socketDesc, 1);

	while(running)
	{
		// accept one connection
		struct sockaddr_rc client_addr = { 0 };
		socklen_t opt = sizeof(client_addr);
		int client = accept(socketDesc, (struct sockaddr*)&client_addr, &opt);

		// print MAC address of the connection received
		char miniBuf[SMALL_BUFFER_SIZE] = { 0 };
		ba2str(&client_addr.rc_bdaddr, miniBuf);
		printf("bluetooth: accepted connection from %s\n", miniBuf);
		memset(miniBuf, 0, sizeof(miniBuf));

		// read data from the client
		char buf[RECEIVE_BUFFER_SIZE] = { 0 };

		// perform a blocking read
		int bytesRead = read(client, buf, sizeof(buf));
		if (bytesRead > 0) {
			printf("bluetooth: received [%s]\n", buf);
		}
		else {
			// potentially lost connection or error, check error codes
		}

		// always remember to close the connections
		// especially when the connection is lost for some reason
		close(client);
	}

	close(socketDesc);

	return NULL;
}

void Bluetooth_stopServer(void)
{
	running = false;
	pthread_join(bluetoothThreadId, NULL);
}


