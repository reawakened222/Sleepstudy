#include "stringHelperFuncs.h"
#include <stdlib.h>
#include <string.h>         //strlen, strcmp
#include <ctype.h>          //isdigit, isspace
int stringBeginsWith_MatchCase(char* src, char* target, int bMatchCase)
{
    size_t sourceLength, targetLength;
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
#define MAXNUMLENGTH 32
long int strToNum(char* nrStr, char** afterNum)
{
    size_t i = 0;
    size_t length = strlen(nrStr);
    char numStr[MAXNUMLENGTH + 1];
    for(i = 0; i < length && (isdigit(nrStr[i]) || isspace(nrStr[i])); i++) {}
    memcpy(numStr, nrStr, i);
    numStr[i] = '\0';
    if(afterNum != NULL)
        *afterNum = nrStr + (i + 1);
    return atol(nrStr);
}
