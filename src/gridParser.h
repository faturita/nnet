#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRID_SIZE_X 8
#define GRID_SIZE_Y 8

#define MAX_BUFFER	256

extern int N;
extern int P;

int *getGridMemory(char *sFileName, int iMemoryIndex);
void showGridMemory(int *gridMemory);

