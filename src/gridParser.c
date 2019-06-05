#include "gridParser.h"


/*
 * Muestra por STDOUT la codificacion de la memoria Y en una grilla rectangular.
 *
 * Parametros:
 *		Y 		Memoria.
 */
void showGridMemory(int *Y)
{
    int i;
    char cElement;

    for (i=0;i<GRID_SIZE_X*GRID_SIZE_Y;i++) {
        cElement = (Y[i]==1) ? '#' : ' ';
        printf ("%c",cElement);
        if (((i +1) % GRID_SIZE_X) == 0)
            printf ("\n");
    } 
}

/*
 * Mapea una grilla de caracteres grabada en el archivo sFileName, en la posicion
 * iMemoryIndex (en relacion a los separadores '-' especificados en el mismo) a una
 * memoria {1,-1}
 *
 * Parametros:
 *		sFileName	Nombre del archivo de configuracion
 *		iMemoryIndex	Posicion dentro del archivo.
 */
int *getGridMemory(char *sFileName, int iMemoryIndex)
{
    FILE *pfGridDFile;
    int *YMemory;
    int inx=0;
    int iny=0;
    char memBuffer[MAX_BUFFER];
    int bFound=0;

    pfGridDFile = fopen(sFileName,"r");
    if (pfGridDFile == NULL) {
        printf ("No se ha encontrado el archivo de configuracion %s.\n",sFileName);
        exit(-1);
    }

    YMemory = (int *)malloc(GRID_SIZE_X*GRID_SIZE_Y*sizeof(int));

    if (YMemory == NULL) {
        printf ("Memoria insuficiente.\n");
        exit(-1);
    }

    fgets(memBuffer,GRID_SIZE_X+2, pfGridDFile);

    iny = 0;
    while ( !feof(pfGridDFile) ) {
        // Control del formato de cada registro de la grilla
        if (memBuffer[GRID_SIZE_X]!='\n') {
            printf("El formato del archivo de grilla debe ser 8x8 para las grillas con 8 caracteres '#',' ','-' seguidos de un \n.");
            exit(-1);
        }

        memBuffer[GRID_SIZE_X]='\0';

        // Verificacion separador de memorias en el archivo de configuracion de grillas.
        if (memBuffer[0] != '-') {
            // La grilla especificada se encuentra en el archivo.
            if (iMemoryIndex==0) {
               bFound=1;
               // Se carga la fila convirtiendo los valores de la grilla en 1 y -1.
               for(inx=0;inx<GRID_SIZE_X;inx++) {
                    //printf ("%d\n",(iny*GRID_SIZE_X+inx));
                    if (memBuffer[inx]=='#') {
                        YMemory[iny*GRID_SIZE_X+inx]=1;
                    } else {
                        YMemory[iny*GRID_SIZE_X+inx]=-1;
                    }
                }
            iny++; // Una nueva fila.
            }
        } else {
            iMemoryIndex--;     // Un nuevo patron
            iny=0;              // Se resetean las filas para el nuevo patron.
        }
        fgets(memBuffer,GRID_SIZE_X+2, pfGridDFile);
    }

    if (!bFound) {
        printf ("La memoria especificada no se encuentra en el archivo de configuracion...\n");
        //Error el dato especificado no se encontro.
        exit(-1);
    }

    fclose(pfGridDFile);

    return (YMemory);
}

/**
 * Main para testing...
 */
int main_test(int argc, char *argv[])
{
    int *Y;
    int i;

    Y = getGridMemory("grid.data.conf",1);

    for (i=0;i<GRID_SIZE_X*GRID_SIZE_Y;i++) {
        printf ("%d,",Y[i]);
        if (((i +1) % GRID_SIZE_X) == 0)
        printf ("\n");
    }

    return 1;
}
