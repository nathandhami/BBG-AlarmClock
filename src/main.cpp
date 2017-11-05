#include "udp.h"
#include "alarm.h"

int main (int argc, char *argv []) {
	startProgram();
	UDP_startServer();


	while(true) {
		waitDelay(1, 0);
	}

	UDP_stopServer();
}
