// udpserver.h
// UDP Server Thread module

#ifndef _UDPSERVER_H_
#define _UDPSERVER_H_

// Start listening to the UDP port on a separate thread.
void UDP_startServer(void);

void UDP_triggerAlarm(_Bool qType, char* question, char* answer);

void UDP_cleanup(void);

#endif
