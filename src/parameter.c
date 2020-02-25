#include "parameter.h"

/**
 * Obtiene un tag pero no cierra el archivo (para leer mucha info en secuencia)
 *
 **/
void getQuickValue(char *buffer, char *searchKey, char *filename) {
    static FILE *pf=NULL;
    char vAux[MAXSIZE];
    char elements[MAXSIZE];
    char key[MAXSIZE];
    int keylen;

    // Inicializar valor con ""
    strcpy(buffer,"");

    if (pf == NULL)
        pf = fopen(filename,"r");

    if (pf == NULL) {
        printf ("Archivo de configuracion (%s) no encontrado.\n",filename);
        return;
    }


    fgets(vAux, MAXSIZE, pf);
    while ( !feof(pf) ) {
        if (vAux[0] == '#' || strlen(vAux) < 3) {
            fgets(vAux,MAXSIZE, pf);
            continue;
        }

        strcpy(elements, (char *)strstr(vAux, "="));
        keylen = strlen(vAux)-strlen(elements);
        strncpy(key, vAux, keylen);
        key[keylen]='\0';

        if (strcmp(searchKey,key)==0) {
            strcpy(buffer,elements+1);
            // Elimino el enter.
            buffer[strlen(elements+1)-1]='\0';
            break;
        }

        fgets(vAux, MAXSIZE, pf);
    }

}

/**
 * Devuelve un valor asociado a una clave en un archivo de configuracion
 *
 * buffer		Buffer de tamanio maximo MAXSIZE donde se almacena el valor
 * searchKey	Clave a buscar
 * filename		Archivo de configuracion
 *
 **/
void getValue(char *buffer, char *searchKey, char *filename) {
    FILE *pf;
    char vAux[MAXSIZE];
    char elements[MAXSIZE];
    char key[MAXSIZE];
    int keylen;

    // Inicializar valor con ""
    strcpy(buffer,"");

    pf = fopen(filename,"r");

    if (pf == NULL) {
        printf ("Archivo de configuracion (%s) no encontrado.\n",filename);
        exit(-1);
    }

    fgets(vAux, MAXSIZE, pf);
    while ( !feof(pf) ) {
        // Eliminates comments
        if (vAux[0] == '#' || strlen(vAux) < 3) {
            fgets(vAux,MAXSIZE, pf);
            continue;
        }

        if (strstr(vAux, "=") != NULL)
        {
            strcpy(elements, (char *)strstr(vAux, "="));
            keylen = strlen(vAux)-strlen(elements);
            strncpy(key, vAux, keylen);
            key[keylen]='\0';

            if (strcmp(searchKey,key)==0) {
                strcpy(buffer,elements+1);
                // Elimino el enter.
                buffer[strlen(elements+1)-1]='\0';
                break;
            }
        }

        fgets(vAux, MAXSIZE, pf);
    }

    fclose(pf);
}



/**
 * Lee un token posicionalmente del archivo de configuracion
 *
 **/
void readToken(char vAux[MAXSIZE],FILE *pf ) {
    if (feof(pf)) {
        vAux = "8";
        return;
    }
    fgets(vAux, MAXSIZE, pf);
    while ( !feof(pf) && vAux[0] == '#' ) {
        fgets(vAux, MAXSIZE, pf);
    }
}

void getParameter(int *N, int *P, int *LC, int *N_MIN, int *N_MAX, int *N_INTERVAL) {
    getParameterWithFileName("parameter.conf", N, P, LC, N_MIN, N_MAX, N_INTERVAL);
}

/**
 * Lee los parametros del archivo de configuracion "parameter.conf"
 **/
void getParameterWithFileName(char sParameterFileName[], int *N, int *P, int *LC, int *N_MIN, int *N_MAX, int *N_INTERVAL) {
    FILE *pf;
    char vAux[MAXSIZE];

    pf = fopen(sParameterFileName,"rw+");

    if (pf == NULL) {
        printf ("Archivo de configuracion (parameter.conf) no encontrado.\n");
        *N = DEFAULTN;
        *P = DEFAULTP;
        printf ("Default (N = %d, P = %d).",*N,*P);
        return;
    }

    readToken(vAux, pf);

    *N = atoi(vAux);

    readToken(vAux,  pf);

    *P = atoi(vAux);

    readToken(vAux, pf);

    *LC = atoi(vAux);

    readToken(vAux, pf);
    *N_MIN = atoi(vAux);
    
    readToken(vAux,pf);
    *N_MAX = atoi(vAux);
    
    readToken(vAux, pf);
    *N_INTERVAL = atoi(vAux);
    
    fclose(pf);
}


