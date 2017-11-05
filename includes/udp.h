#ifndef _UDPSERVER_H_
#define _UDPSERVER_H_

// Start listening to the UDP port on a separate thread.
void UdpListener_startListening(void);

void UdpListener_cleanup(void);

#endif
