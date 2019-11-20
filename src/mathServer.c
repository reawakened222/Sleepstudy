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

#define MESSAGE_HANDLED 1
#define MESSAGE_NOT_HANDLED 0

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
static int logCount = 0;
static void logAndPrint(char* text)
{    
    const time_t startTime = time(NULL);
    struct tm* localTime = gmtime(&startTime);
    char buf[100];
    makeTimeStamp(localTime, buf);
    fprintf(logFile_p, "%s: %s", buf, text);
    printf("%s\n", text);
    logCount++;
    if(logCount >= 10)
    {
        fflush(logFile_p);
        logCount = 0;
    }
}
static int getResponse(int workerID, char* clientMsg, char* response)
{
    int handled = MESSAGE_NOT_HANDLED;
    /* Do something based on the messages received */
    if(stringBeginsWith(clientMsg, "Fibo(") == 0)
    {        
        /* Assume msg is Fibo(<nr>), so we extract the number */
        int nr = strToNum(&clientMsg[5], NULL);

        /* Calculate and send response */
        long int fib = calcFibo(nr);
        printf("Calculated fibo(%d)=%ld\n", nr, fib);
        sprintf(response, "%ld", fib);
        handled = MESSAGE_HANDLED;
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
        handled = MESSAGE_HANDLED;
    }
    return handled;
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
            if (getResponse(actorNr, command, response[actorNr])){
                //zstr_send (pipe, response[actorNr]);
            }
            else {
                puts ("E: invalid message to actor");
            }
        }
        freen (command);
        zmsg_destroy (&msg);
    }
}
static void teardown()
{
    printf("Shutting down server ... Goodbye!\n");
    // Cleanup and close up stuff here
    zsock_destroy(&responder);
    for(int i = 0; i < NR_OF_WORKERS; i++)
    {
        zactor_destroy(&background_server_workers[i]);
    }
    fclose(logFile_p);
}
static void signal_handler_callback(int signum)
{
    printf("Caught signal %d\n",signum);
    teardown();    
    // Terminate program
    exit(signum);
}

static int serverMessageHandler(char* msg, char server_response[MSGSIZE+1])
{
    if(strcasecmp(msg, "hello") == 0)
    {
        sprintf(server_response, "HELLO");
        return 1;
    }
    else if(strcasecmp(msg, "terminate") == 0)
    {
        logAndPrint("SERVER SHUTDOWN INITIATED\n");
        running = SERVER_STOPPED;   /* STOPPING SERVER */
        server_response[0] = '\0';
        return 1;
    }
    return 0;
}
static zsock_t* makeSocket(const char* connectionString, int socketType)
{
    zsock_t * sock = zsock_new(socketType);
    if (sock == NULL)
    {
        return NULL;
    }
    int rc = zsock_bind(sock, connectionString);

    if (rc != 5555)
    {
        printf("zsock_bind returned %d\n", rc);
    }
    return sock;
}
static int setupServer()
{
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
}
int printServerStatus()
{

}
int main(void)
{
    /* Set some signal callbacks, for a more graceful server shutdown */
    signal(SIGINT, signal_handler_callback);
    signal(SIGSEGV, signal_handler_callback);
    
    setupServer();
    //  Socket to talk to clients
    responder = makeSocket("tcp://*:5555", ZMQ_REP);
    if (responder == NULL)
    {
        printf("Socket not created properly.\n");
    }
    char logMsg[1024];
    int nextIdleWorker = 0;

    int i = 0;
    /* Spawn a number of workers */
    for(i = 0; i < NR_OF_WORKERS; i++)
    {
        background_server_workers[i] = zactor_new(server_worker_thread, (void*)i);
        if(background_server_workers[i] == NULL)
        {
            printf("Actor %d could not be created\n", i);
        }
    }
    printf("Waiting for messages ...\n");
    char server_response[MSGSIZE+1] = {0};
    while (running == SERVER_RUNNING)
    {
        int messageType = 0;
        char* client_msg = zstr_recv(responder);
        if(client_msg != NULL && strcmp(client_msg, "") != 0)
        {
            sprintf(logMsg, "MESSAGE RECEIVED: %s\n", client_msg);
            logAndPrint(logMsg);

            /* Handle Server-specific messages */
            if(serverMessageHandler(client_msg, server_response))   //Returns true if server handled message itself
            {
                /* Check if the server should send something back to the client */
                if(server_response[0] != '\0')
                {
                    zstr_send(responder, server_response);
                    sprintf(logMsg, "RESPONSE SENT: %s\n", server_response);
                    logAndPrint(logMsg);
                }
            }
            else 
            {
                sprintf(logMsg, "Forwarding to actor %d - %s\n", nextIdleWorker, client_msg);
                logAndPrint(logMsg);
                /* Send it to an actor, check response eventually */
                zstr_send(background_server_workers[nextIdleWorker], client_msg);
                nextIdleWorker = (nextIdleWorker + 1) % NR_OF_WORKERS;
            }

            zstr_free(&client_msg);
        }
        
        for(i = 0; i < NR_OF_WORKERS; i++)
        {
            //char * worker_response = zstr_recv_nowait(background_server_workers[nextIdleWorker]);
            if(response[i][0] != '\0'){
                sprintf(logMsg, "Actor %d is done. Sending result <%s> back to client\n", i, response[i]);
                logAndPrint(logMsg);
                /* Do something with response */
                zstr_send(responder, response[i]);
                //zstr_free(&worker_response);
                response[i][0] = '\0';
            }
        }
    }
    teardown();
    return 0;
}
