#ifndef _SERVER_H_
#define _SERVER_H_

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