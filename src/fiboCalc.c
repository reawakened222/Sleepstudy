#include <time.h>
#include <stdio.h>
long int getFibo(int nr)
{
    if(nr < 3)
    return 1;

    return getFibo(nr-1) + getFibo(nr-2);
}
int fastFibo(int n)
{
    int a = 1, b = 1, c = 1, i;
	for (i = 3; i <= n; i++) {
		c = a + b;
		a = b;
		b = c;
	}
    return c;
}
//#define CLOCKTEST
#ifdef CLOCKTEST
#define ITERATIONS 10
int main(int argc, char** argv)
{
    int i = 0;

    time_t begin, end;
    if(argc <= 0)
    return -1;
    int fiboNr = atoi(argv[1]);
    volatile long int fib;
    printf("Calculating fibo(%d) %d times\n", fiboNr, ITERATIONS);
    begin = clock();
    for(i = 0; i < ITERATIONS; i++)
    {
        fib = getFibo(fiboNr);
    }
    end = clock();
    printf("%d iterations took %f seconds\n", ITERATIONS, ((double) end - begin) / CLOCKS_PER_SEC);
    printf("Average time: %f seconds\n", ((double) end - begin) / CLOCKS_PER_SEC / ITERATIONS);

    return 0;
}
#else
int main(int argc, char** argv)
{
    int i = 0;

    if(argc <= 0)
    return -1;
    int maxFiboNr = atoi(argv[1]);
    for(i = 1; i < maxFiboNr; i++)
    {
        printf("%d\n", fastFibo(i));
    }

    return 0;
}
#endif