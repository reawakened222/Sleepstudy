#include "stringHelperFuncs.h"
#include <stdio>
int stringBeginsWith_MatchCase(char* src, char* target, int bMatchCase)
{
    int sourceLength, targetLength;
    sourceLength = strlen(src);
    targetLength = strlen(target);
    /* Check if substring is longer than target string */
    if(sourceLength < targetLength)
    {
        /* If it is, it cannot be a substring of src */
        return -1;
    }
    if(bMatchCase)
    {
        return strncmp (src, target, targetLength);
    }
    else
    {
        return strncasecmp(src, target, targetLength);
    }
}
int stringBeginsWith(char* src, char* target)
{
    return stringBeginsWith_MatchCase(src, target, 0);
}
long int strToNum(char* nrStr, char** afterNum)
{
    int i = 0;
    int length = strlen(nrStr);
    for(i = 0; i < length && (isdigit(nrStr[i]) ||isspace(nrStr[i])); i++) {}
    char numStr[i+1];
    memcpy(numStr, nrStr, i);
    numStr[i] = '\0';
    if(afterNum != NULL)
        *afterNum = nrStr + (i + 1);
    return atol(nrStr);
}