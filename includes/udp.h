#ifndef _UDPSERVER_H_
#define _UDPSERVER_H_
// Begin/end the background thread UDP server.
void UDP_startServer(void);
void UDP_stopServer(void);
bool UDP_checkStop(void);
#endif
