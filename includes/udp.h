#ifndef _UDPSERVER_H_
#define _UDPSERVER_H_

// Start listening to the UDP port on a separate thread.
void UDP_startServer(void);

void UDP_cleanup(void);

#endif