//  Hello World server
#include <czmq.h>

#include <stdlib.h>
#include <signal.h>
#include <string.h>

#ifdef _WINDOWS
#define strcasecmp stricmp 

#endif
#include <stdio.h>
#include <ctype.h>
#include <time.h>

#include "Server.h"

#include "mathFuncs.h"

#define PORTNR 5555

/* Message levels, determines what server should do when messages are received */
#define USERMSG 1
#define ADMINMSG 2

int running = SERVER_RUNNING;
zsock_t *responder;
FILE* file_p;

char* getClientConnectionString()
{
    return "tcp://localhost:5555";
}
int getServerStatus()
{
    return running;
}

int makeTimeStamp(struct tm* time, char* result)
{
    sprintf(result, "%d-%02d-%02d_%02d%02d%02d", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);
    return 0;
}
void logAndPrint(char* text)
{    
    const time_t startTime = time(NULL);
    struct tm* localTime = gmtime(&startTime);
    char buf[100];
    makeTimeStamp(localTime, buf);
    fprintf(file_p, "%s: %s", buf, text);
    printf("%s\n", text);
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
void signal_handler_callback(int signum)
{
    printf("Caught signal %d\n",signum);
    printf("Shutting down server ... Goodbye!\n");
    // Cleanup and close up stuff here
    zsock_destroy(&responder);
    fclose(file_p);
    // Terminate program
    exit(signum);
}
int main(void)
{
    signal(SIGINT, signal_handler_callback);
    signal(SIGSEGV, signal_handler_callback);
    //  Socket to talk to clients
    responder = zsock_new(ZMQ_REP);
    const time_t startTime = time(NULL);
    struct tm* localTime = gmtime(&startTime);
    char logFileName[100] = "serverlog_";
    makeTimeStamp(localTime, &logFileName[strlen(logFileName)]);
    sprintf(&logFileName[strlen(logFileName)], ".log");
    file_p = fopen(logFileName, "w+");
    if(file_p == NULL)
    {
        printf("Log file could not be created.\n");
    }
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
    char logMsg[100];
    while (running == SERVER_RUNNING)
    {
        int messageType = 0;
        
        char* str = zstr_recv(responder);
        sprintf(logMsg, "MESSAGE RECEIVED: %s\n", str);
        logAndPrint(logMsg);
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
        sprintf(logMsg, "RESPONSE SENT: %s\n", response);
        logAndPrint(logMsg);
        zstr_free(&str);
    }
    printf("Shutting down server ... Goodbye!\n");
    return 0;
}