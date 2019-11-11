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
#include "stringHelperFuncs.h"

/* Message levels, determines what server should do when messages are received */
#define USERMSG 1
#define ADMINMSG 2

/* Global variables */
static int running = SERVER_RUNNING;   //Server status variable. Right now just a way to stop the running from within the program
static zsock_t *responder;             //Socket for server connections
static FILE* logFile_p;                //Log file pointer

/*  */
static char* getClientConnectionString()
{
    return "tcp://localhost:5555";
}
int getServerStatus()
{
    return running;
}

static int makeTimeStamp(struct tm* time, char* result)
{
    sprintf(result, "%d-%02d-%02d_%02d%02d%02d", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);
    return 0;
}
static void logAndPrint(char* text)
{    
    const time_t startTime = time(NULL);
    struct tm* localTime = gmtime(&startTime);
    char buf[100];
    makeTimeStamp(localTime, buf);
    fprintf(logFile_p, "%s: %s", buf, text);
    printf("%s\n", text);
}
static int getResponse(char* clientMsg, char* response)
{
    int result = 0;

    /* Do something based on the messages received */
    if(stringBeginsWith(clientMsg, "Fibo(") == 0)
    {        
        /* Assume msg is Fibo(<nr>), so we extract the number */
        int nr = strToNum(&clientMsg[5], NULL);

        /* Calculate and send response */
        long int fib = calcFibo(nr);
        printf("Calculated fibo(%d)=%ld\n", nr, fib);
        sprintf(response, "%ld", fib);
        result = USERMSG;
    }
    else if(stringBeginsWith(clientMsg, "Pascal(") == 0)
    {
        /* Assume message looks like Pascal(<k>,<n>) */
        char * afterFirstNum;
        long int firstNum = strToNum(&clientMsg[7], &afterFirstNum);
        /* afterFirstNum should point past the first number, i.e. the comma */
        long int secNum = strToNum(afterFirstNum+1, NULL);
        long int pascalNr = calcPascal(firstNum, secNum);

        printf("Calculated Pascal(%ld, %ld)=%ld\n", firstNum, secNum, pascalNr);
        sprintf(response, "%ld", pascalNr);
        result = USERMSG;
    }
    else if(stringBeginsWith_MatchCase(clientMsg, (char*)"Hello", 1) == 0)
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

#define NR_OF_WORKERS 10
#define MSGSIZE 100
static zactor_t* background_server_workers[NR_OF_WORKERS];
static char response[NR_OF_WORKERS][MSGSIZE];
static void server_worker_thread(zsock_t *pipe, void *args)
{
    //  Do some initialization
    zsock_signal (pipe, 0);

    bool terminated = false;
    long int actorNr = (long int) args;
    printf("Started Actor nr %ld\n", actorNr);
    while (!terminated) {
        zmsg_t *msg = zmsg_recv (pipe);
        if (!msg)
            break;              //  Interrupted
        char *command = zmsg_popstr (msg);
        //  All actors must handle $TERM in this way
        if (streq (command, "$TERM"))
            terminated = true;
        else {
            //  This is an example command for our test actor
            if (getResponse(command, response[actorNr]))
                zstr_send (pipe, response[actorNr]);
            else {
                puts ("E: invalid message to actor");
            }
        }
        freen (command);
        zmsg_destroy (&msg);
    }
}

static void signal_handler_callback(int signum)
{
    printf("Caught signal %d\n",signum);
    printf("Shutting down server ... Goodbye!\n");
    // Cleanup and close up stuff here
    zsock_destroy(&responder);
    fclose(logFile_p);
    // Terminate program
    exit(signum);
}
int main(void)
{
    /* Set some signal callbacks, for a more graceful server shutdown */
    signal(SIGINT, signal_handler_callback);
    signal(SIGSEGV, signal_handler_callback);

    /* Timing data */
    const time_t startTime = time(NULL);
    struct tm* localTime = gmtime(&startTime);

    /* Make the log file */
    char logFileName[100] = "serverlog_";
    makeTimeStamp(localTime, &logFileName[strlen(logFileName)]);
    sprintf(&logFileName[strlen(logFileName)], ".log");
    logFile_p = fopen(logFileName, "w+");
    if(logFile_p == NULL)
    {
        printf("Log file could not be created.\n");
    }

    //  Socket to talk to clients
    responder = zsock_new(ZMQ_ROUTER);
    if (responder == NULL)
    {
        printf("Socket not created properly.\n");
    }
    int rc = zsock_bind(responder, "tcp://*:5555");

    if (rc != 5555)
    {
        printf("zsock_bind returned %d\n", rc);
    }
    char logMsg[100];
    int nextIdleWorker = 0;
    /* Spawn a number of workers */
    for(int i = 0; i < NR_OF_WORKERS; i++)
    {
        background_server_workers[i] = zactor_new(server_worker_thread, (void*)i);
    }
    printf("Waiting for messages ...\n");
    while (running == SERVER_RUNNING)
    {
        int messageType = 0;
        
        char* str = zstr_recv(responder);
        sprintf(logMsg, "MESSAGE RECEIVED: %s\n", str);
        logAndPrint(logMsg);
        
        zstr_send(background_server_workers[nextIdleWorker], str);
        char * client_response = zstr_recv(background_server_workers[nextIdleWorker]);
        /* Handle responses in a separate function */
        //messageType = getResponse(str, response);

        /* Take extra actions depending on the type of message */
        if(messageType == USERMSG)
        {
            /* Send response back */
            zstr_send(responder, client_response);
        }
        else if(messageType == ADMINMSG)
        {
            printf("Admin message received: %s\n", str);
        }
        else
        {
            zstr_send(responder, "Sorry, I don't understand that request.\n");
        }
        sprintf(logMsg, "RESPONSE SENT: %s\n", client_response);
        logAndPrint(logMsg);
        zstr_free(&str);
    }
    printf("Shutting down server ... Goodbye!\n");
    for(int i = 0; i < NR_OF_WORKERS; i++)
    {
        zactor_destroy(&background_server_workers[i]);
    }
    return 0;
}
