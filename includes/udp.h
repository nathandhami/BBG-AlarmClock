// udpserver.h
// UDP Server Thread module

#ifndef _UDPSERVER_H_
#define _UDPSERVER_H_

// Start listening to the UDP port on a separate thread.
void UDP_startServer(void);

void UDP_triggerAlarm(bool qType, const char* question, const char* op1, 
		const char* op2, const char* op3, const char* op4);
void UDP_stopAlarm(void);

void UDP_cleanup(void);

#endif
