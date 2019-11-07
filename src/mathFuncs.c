#include "mathFuncs.h"

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
    if(k > n)
    {
        return -1;
    }
    else if(n == 0 && k == 0)
    {
        return 1;
    }
    else if(k == 0 || k == n)
    {
        return 1;
    }
    return calcPascal(n-1, k-1) + calcPascal(n-1, k);
}