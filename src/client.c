//  Hello World client
#include <czmq.h>
#include <stdlib.h> //for EXIT_FAILURE
#define FIBONR 42
#define FIBORESULT "433494437"
#define Secs (1000)
int nullableStrCmp(char* str1, char* str2)
{
    if(str1 == NULL)
    {
        return -2;
    }
    else if(str2 == NULL)
    {
        return 2;
    }
    return strcmp(str1,str2);
}
int main(void)
{    
    printf("Connecting to hello world server…\n");
    zsock_t *requester = zsock_new(ZMQ_REQ);
    if (requester == NULL)
    {
        printf("Socket not created properly.\n");
    }
    zsock_connect(requester, "tcp://localhost:5555");

    int request_nbr;
    char buf[20];
    int failedAsserts = 0;
    for (request_nbr = 0; request_nbr < 5; request_nbr++)
    {
        sprintf(buf, "Fibo(%d)", FIBONR);
        printf("Sending %s\n", buf);
        
        zstr_send(requester, buf);
        zclock_sleep(2500);
        char *str = zstr_recv_nowait(requester);
        failedAsserts += (nullableStrCmp(str, FIBORESULT) == 0) ? 0 : 1;
        
        printf("Response: %s\n", str);
        zstr_free(&str);
    }
    zstr_send(requester, "SHUTDOWN");
    zsock_destroy(&requester);
    return (failedAsserts != 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}