#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "Server.h"
#include "debugFuncWrappers.h"
#include <czmq.h>
#include "CuTest.h"

/* Some test framework functions */
zsock_t* clientSocket = NULL;

#define CONNSTRING "tcp://localhost:5555"

int beforeTest()
{
    if(clientSocket == NULL)
    {
        clientSocket = zsock_new(ZMQ_REQ);
        zsock_connect(clientSocket, CONNSTRING);
    }
    return 0;
}

int afterTest()
{
    zsock_destroy(&clientSocket);
    clientSocket = NULL;
    return 0;
}

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
        return;
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

/* END TEST FRAMEWORK FUNCTIONS */

#define FIB32 "2178309"
void TestFunctionalityCalcFib32(CuTest * tc)
{
    beforeTest();
    sendMessage("Fibo(32)");
    char* resMsg = NULL;
    getMessage(&resMsg);
    CuAssertStrEquals(tc, FIB32, resMsg);
    free(resMsg);
    afterTest();
}

#define FIB40 "102334155"
void TestFunctionalityCalcFib40(CuTest * tc)
{
    beforeTest();
    sendMessage("Fibo(40)");
    char* resMsg = NULL;
    getMessage(&resMsg);
    CuAssertStrEquals(tc, FIB40, resMsg);
    free(resMsg);
    afterTest();
}
#define FIB45 "1134903170"
void TestFunctionalityCalcFib45(CuTest * tc)
{
    beforeTest();
    sendMessage("Fibo(45)");
    char* resMsg = NULL;
    getMessage(&resMsg);
    CuAssertStrEquals(tc, FIB45, resMsg);
    free(resMsg);
    afterTest();
}
CuSuite * ServerBasicFunctionalitySuite(void)
{
    CuSuite * s = CuSuiteNew();
    SUITE_ADD_TEST(s, TestFunctionalityCalcFib32);
    SUITE_ADD_TEST(s, TestFunctionalityCalcFib40);
    SUITE_ADD_TEST(s, TestFunctionalityCalcFib45);
    return s;
}