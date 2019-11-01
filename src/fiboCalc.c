#include <time.h>
#include <stdio.h>
long int getFibo(int nr)
{
    if(nr < 3)
    return 1;

    return getFibo(nr-1) + getFibo(nr-2);
}

int main(int argc, char** argv)
{
    int i = 0;

    time_t begin, end;
    if(argc <= 0)
    return -1;
    int fiboNr = atoi(argv[1]);
    volatile long int fib;
    printf("Calculating fibo(%d)\n", fiboNr);
    begin = clock();
    for(i = 0; i < 100; i++)
    {
        fib = getFibo(fiboNr);
    }
    end = clock();
    printf("100 iterations took %lld seconds\n", (end - begin) / CLOCKS_PER_SEC);
    printf("%ld", fib);

    return 0;
}