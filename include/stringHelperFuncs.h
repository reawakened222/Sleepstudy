#ifndef STRING_HELPER_FUNCS_H
#define STRING_HELPER_FUNCS_H 0

int stringBeginsWith_MatchCase(char* src, char* target, int bMatchCase);
int stringBeginsWith(char* src, char* target);
long int strToNum(char* nrStr, char** afterNum);

#endif
