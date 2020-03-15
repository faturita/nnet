#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSIZE 256
#define DEFAULTN 8
#define DEFAULTP 3

void getParameter(int *N, int *P, int *LC, int *N_MIN, int *N_MAX, int *N_INTERVAL) ;
void getParameterWithFileName(char sParameterFileName[], int *N, int *P, int *LC, int *N_MIN, int *N_MAX, int *N_INTERVAL) ;
void getValue(char *,char *, char *);
void getQuickValue(char *, char *, char *, char *defaultValue);
void getDefaultedValue(char *buffer, char *searchKey, char *filename, char *defaultValue);

