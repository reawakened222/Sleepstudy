#ifndef DEBUGFUNCWRAPPERS_H
#define DEBUGFUNCWRAPPERS_H

int debugPrint(char* text, char* file, int line);
#define PRINT_DBG(text) debugPrint((text), __FILE__, __LINE__)

#endif