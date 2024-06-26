extern FILE *pfLogFile;



/**
 * Funcion de loggin sobre file pointer pfLogFile
 *
 * buffer			Char con el mensaje a loguear.
 **/
void logInfo(char *buffer);

/**
 * Devuelve verdadero si esta activado el logging (el archivo de log existe)
 **/
int isLogging();


