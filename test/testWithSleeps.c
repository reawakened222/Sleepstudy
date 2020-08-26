#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "Server.h"
#include "debugFuncWrappers.h"
#include <czmq.h>
#include "CuTest.h"
#include "testSuites.h"

/* Some test framework functions */
zsock_t* clientSocket = NULL;

#define CONNSTRING "tcp://localhost:5555"

int sendMessage(char* msg)
{
    return zstr_send(clientSocket, msg);
}

int getMessage(char** msg_p)
{
    char* msg = zstr_recv(clientSocket);
    int msgLen = strlen(msg);
    if(msgLen <= 0)
    {
        return -1;
    }
    *msg_p = malloc(sizeof(char) * msgLen + 1);
    if(*msg_p == NULL)
    {
        return -1;
    }
    memcpy(*msg_p, msg, sizeof(char) * msgLen + 1);
    zstr_free(&msg);
    return msgLen;
}
int getMessageNoWait(char** msg_p)
{
    char* msg = zstr_recv_nowait(clientSocket);
    if(msg == NULL)
        return -1;
    int msgLen = strlen(msg);
    if(msgLen <= 0)
    {
        return -1;
    }
    *msg_p = malloc(sizeof(char) * msgLen + 1);
    if(*msg_p == NULL)
    {
        return -1;
    }
    memcpy(*msg_p, msg, sizeof(char) * msgLen + 1);
    zstr_free(&msg);
    return msgLen;
}
int sleep_MS(int msToSleep)
{
    zclock_sleep(msToSleep);
    return 0;
}

int beforeTest()
{
    printf("Running beforetest\n");
    if(clientSocket == NULL)
    {
        clientSocket = zsock_new(ZMQ_REQ);
        zsock_connect(clientSocket, CONNSTRING);
    }
    return 0;
}

/* Consume and throw away any waiting messages */
int consumeMsg()
{
    while(zstr_recv(clientSocket)) {}
    return 0;
}
int afterTest()
{
    consumeMsg();
    zsock_destroy(&clientSocket);
    clientSocket = NULL;
    return 0;
}
/* END TEST FRAMEWORK FUNCTIONS */

/* A sanity environment test to check if the server is actually running */
void testServerConnection(CuTest* tc)
{
    beforeTest();
    sendMessage("Hello");
    /* Give the server some time to respond to this message 
       If it does not manage within this time, it means it is not in a state where it would make sense to run the remaining tests
    */
    sleep_MS(1000);
    char* response = NULL;
    getMessageNoWait(&response);
    if(response == NULL)
    {
        printf("Server is not responding to messages within proper time. Terminate test session and check status.\n");
        exit(1);
    }
    else{
        CuAssertStrEquals(tc, "HELLO", response);
        free(response);
        afterTest();
    }
}

#define FIB32 "2178309"
void testFunctionalityCalcFib32(CuTest * tc)
{
    beforeTest();
    sendMessage("Fibo(32)");
    char* resMsg = NULL;
    sleep_MS(50);
    /* Note: This is to test the detection and fixing of flaky delays.
     * NoWait is Bad, and here we should tell the user!!! */
    getMessageNoWait(&resMsg);
    CuAssertStrEquals(tc, FIB32, resMsg);
    free(resMsg);
    afterTest();
}

#define FIB40 "102334155"
void testFunctionalityCalcFib40(CuTest * tc)
{
    beforeTest();
    sendMessage("Fibo(40)");
    char* resMsg = NULL;
    sleep_MS(1200); /* No observed runs over 1100ms. Adding ~100ms as safety margin */
    getMessageNoWait(&resMsg);
    CuAssertStrEquals(tc, FIB40, resMsg);
    free(resMsg);
    afterTest();
}

CuSuite * ServerBasicFunctionalitySuite(void)
{
    CuSuite * s = CuSuiteNew();
    SUITE_ADD_TEST(s, testServerConnection);
    SUITE_ADD_TEST(s, testFunctionalityCalcFib32);
    SUITE_ADD_TEST(s, testFunctionalityCalcFib40);
    return s;
}