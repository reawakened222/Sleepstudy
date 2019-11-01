//  Hello World server
#include <czmq.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

#include "Server.h"

#define PORTNR 5555

/* Message levels, determines what server should do when messages are received */
#define USERMSG 1
#define ADMINMSG 2

int running = SERVER_RUNNING;

long int calcFibo(int n)
{
    if (n < 3)
    {
        return 1;
    }
    return calcFibo(n-1) + calcFibo(n-2);
}
int stringBeginsWith(char* src, char* target)
{
    int sourceLength, targetLength;
    sourceLength = strlen(src);
    targetLength = strlen(target);
    /* Check if substring is longer than target string */
    if(sourceLength < targetLength)
    {
        /* If it is, it cannot be a substring of src */
        return -1;
    }
    return strncmp (src, target, targetLength);
}
long int strToNum(char* nrStr)
{
    int i = 0;
    int length = strlen(nrStr);
    for(i = 0; i < length && isdigit(nrStr[i]); i++)
    {

    }
    char numStr[i+1];
    int c = 0;
    memcpy(numStr, nrStr, i);
    numStr[i] = '\0';
    return atol(nrStr);
}
char* getClientConnectionString()
{
    return "tcp://localhost:5555";
}
int getServerStatus()
{
    return running;
}
int getResponse(char* clientMsg, char* response)
{
    int result = 0;

    /* Do something based on the messages received */
    if(stringBeginsWith(clientMsg, "Fibo") == 0)
    {
        /* Assume msg is Fibo(<nr>), so we extract the number */
        int nr = strToNum(&clientMsg[5]);

        /* Calculate the fibonacci nr */
        long int fib = calcFibo(nr);
        printf("Calculated fibo(%d)=%ld\n", nr, fib);
        sprintf(response, "%ld", fib);
        result = USERMSG;
    }
    else if(strcmp(clientMsg, "SHUTDOWN") == 0)
    {
        /* Server got a message to shutdown */
        running = SERVER_STOPPED;
        sprintf(response, "SHUTTING DOWN");
        result = ADMINMSG;
    }
    return result;
}
#define POLLTIME 500
int main(void)
{
    //  Socket to talk to clients
    zsock_t *responder = zsock_new(ZMQ_REP);
    if (responder == NULL)
    {
        printf("Socket not created properly.\n");
    }
    int rc = zsock_bind(responder, "tcp://*:5555");

    if (rc != 5555)
    {
        printf("zsock_bind returned %d\n", rc);
    }
    printf("Waiting for messages ...\n");
    char response[50];
    char timerValue[50];
    int64_t begin = zclock_mono();
    int timeSinceLastMessage;
    int maxIdleTimeMs = 5000;
    while (running == SERVER_RUNNING)
    {
        int messageType = 0;
        
        char* str = zstr_recv(responder);
        /* Handle responses in a separate function */
        messageType = getResponse(str, response);
        if(messageType == USERMSG)
        {
            /* Send response back */
            zstr_send(responder, response);
        }
        else if(messageType == ADMINMSG)
        {
            printf("Admin message received: %s\n", str);
        }
        else
        {
            zstr_send(responder, "Sorry, I don't understand that request.\n");
        }

        zstr_free(&str);
    }
    printf("Shutting down server ... Goodbye!\n");
    return 0;
}