#ifndef HELPFUNC_H_INCLUDED
#define HELPFUNC_H_INCLUDED

#include <stdbool.h>
#include <string.h>
#include <malloc.h>
char *CreateFullPath(char path[], char *name);
int AddSlash(char adir[]);
int Split(char delim, char *string);

#endif // HELPFUNC_H_INCLUDED
