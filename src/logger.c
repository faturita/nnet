#include <stdlib.h>
#include <stdio.h>
#include "logger.h"

char logBuffer[256];

/**
 * Si esta seteada la variable global para logging devuelve VERDADERo
 * sino FALsO.
 *
 **/
int isLogging() {
	if (pfLogFile != NULL)
   	return 1;
   else
   	return 0;
}

/**
 * Log buffer
 **/
void logInfo(char *buffer) {
	if (pfLogFile != NULL ) {
		fputs(buffer, pfLogFile);
   	fflush(pfLogFile);
   }
}


