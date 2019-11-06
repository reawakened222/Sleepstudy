//  Hello World server
#include <czmq.h>
#include <stdlib.h>

#include <string.h>

#ifdef _WINDOWS
#define strcasecmp stricmp 

#endif
#include <stdio.h>
#include <ctype.h>
#include <time.h>

#include "Server.h"

#define PORTNR 5555

/* Message levels, determines what server should do when messages are received */
#define USERMSG 1
#define ADMINMSG 2

int running = SERVER_RUNNING;

long int calcFibo(unsigned int n)
{
    if (n < 3)
    {
        return 1;
    }
    return calcFibo(n-1) + calcFibo(n-2);
}
long int calcPascal(unsigned int n, unsigned int k)
{
    if(n == 0 && k == 0)
    {
        return 1;
    }
    else if(k == 0 || k == n)
    {
        return 1;
    }
    if(k > n)
    {
        return -1;
    }
    return calcPascal(n-1, k-1) + calcPascal(n-1, k);
}
int stringBeginsWith_MatchCase(char* src, char* target, int bMatchCase)
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
    if(bMatchCase)
    {
        return strncmp (src, target, targetLength);
    }
    else
    {
        return strncasecmp(src, target, targetLength);
    }
}
int stringBeginsWith(char* src, char* target)
{
    return stringBeginsWith_MatchCase(src, target, 0);
}
long int strToNum(char* nrStr, char** afterNum)
{
    int i = 0;
    int length = strlen(nrStr);
    for(i = 0; i < length && isdigit(nrStr[i]); i++)
    {

    }
    char numStr[i+1];
    memcpy(numStr, nrStr, i);
    numStr[i] = '\0';
    if(afterNum != NULL)
        *afterNum = nrStr + (i + 1);
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
    if(stringBeginsWith(clientMsg, "Fibo(") == 0)
    {
        
        /* Assume msg is Fibo(<nr>), so we extract the number */
        int nr = strToNum(&clientMsg[5], NULL);

        /* Calculate the fibonacci nr */
        long int fib = calcFibo(nr);
        printf("Calculated fibo(%d)=%ld\n", nr, fib);
        sprintf(response, "%ld", fib);
        result = USERMSG;
    }
    else if(stringBeginsWith(clientMsg, "Pascal(") == 0)
    {
        /* Assume message looks like Pascal(<k>,<n>) */
        char * afterFirstNum;
        int firstNum = strToNum(&clientMsg[7], &afterFirstNum);
        /* afterFirstNum should point past the first number, i.e. the comma */
        int secNum = strToNum(afterFirstNum+1, NULL);
        long int pascalNr = calcPascal(firstNum, secNum);

        printf("Calculated Pascal(%d, %d)=%ld\n", firstNum, secNum, pascalNr);
        sprintf(response, "%ld", pascalNr);
        result = USERMSG;
    }
    else if(stringBeginsWith_MatchCase(clientMsg, "Hello", 1) == 0)
    {
        sprintf(response, "HELLO");
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