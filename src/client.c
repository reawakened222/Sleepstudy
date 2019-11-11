//  Hello World client
#include <czmq.h>
#include <stdlib.h> //for EXIT_FAILURE

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
    int FIBONR = rand() % 20 + 1;
    while(1)
    {
        sprintf(buf, "Fibo(%d)", FIBONR);
        printf("Sending %s\n", buf);
        
        zstr_send(requester, buf);
        char *str = zstr_recv(requester);
        
        printf("Response: %s\n", str);
        zstr_free(&str);
        zclock_sleep(rand() % 1000);
    }
    zstr_send(requester, "SHUTDOWN");
    zsock_destroy(&requester);
    return (failedAsserts != 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}