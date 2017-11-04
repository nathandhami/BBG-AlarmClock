#ifndef _UDPSERVER_H_
#define _UDPSERVER_H_
// Begin/end the background thread UDP server.
void UDP_startServer(void);
void UDP_stopServer(void);
_Bool UDP_checkStop(void);
#endif
