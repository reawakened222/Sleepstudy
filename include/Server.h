#ifndef SERVER_H_
#define SERVER_H_ 0

#include <pthread.h>
#include <czmq.h>
#define CONNECTION_REFUSED (-1)

#define SERVER_STOPPED (-1)
#define SERVER_STARTED (0)
#define SERVER_RUNNING (1)

char* serverGetClientConnectionString();
int resetServer();

int getServerStatus();
#endif
