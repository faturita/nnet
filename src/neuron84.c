/*
 * Simulacion Red Neuronal Hopfield 82
 *
 *
 * La matriz de pesos se calcula forzando que los pesos en la diagonal
 * principal sean 0 para garantizar la estabilidad.
 *
 */

#include <stdio.h>
#include <string.h>

#include "parameter.h"
#include "mathNeuron.h"
#include "arrayView.h"

// Cantidad de iteraciones
#define REPLY_FACTOR (N*N*P)

// Tipo de dato para las salidas de las nueronas.
typedef float neuron;


int N;                  // Cantidad de Neuronas de la red.
int P;                  // Cantidad de palabras a almacenar

int N_MIN;              // Cantidad de neuronas minimas para generar valores para testeo de capacidad.
int N_MAX;              // Cantidad de neuronas maximas para generar valores para testeo de capacidad
int N_INTERVAL;         // Intervalo de crecimiento de cantidad de nueronas de la red al realizar testeo de capacidad.

int LC;                 // Cantidad de conexiones a eliminar



/*
 * Obtiene una memoria de N bits en forma aleatoria.
 */
void getMemory(neuron *E)
{
    int i;
    int iProb;

    for(i=0;i<N;i++) {
        iProb = 1+(int)(2.0*rand()/(RAND_MAX+1.0));
        if (iProb == 1)
            *(E+i) = (rand()/(RAND_MAX+1.0));
        else
            *(E+i) = -(rand()/(RAND_MAX+1.0));
    }

}

/*
 * Muestra la matriz de pesos sinapticos.
 */
void showWeight(float *vVector, int iRow, int iCol)
{
    int i,j;
    printf ("|");

    for (i=0;i<iRow;i++)
        for (j=0;j<iCol;j++) {
            printf ("%8.6f",*(vVector+(i*iCol)+j));
            if (j+1<iCol) printf ("\t");
            if (j+1==iCol) printf ("|\n");
    }

}

/*
 * Muestra por pantalla la memoria E
 */
void showMemory(neuron *E)
{
    int i;
    printf ("[");

    for (i=0;i<N;i++) {
        printf (DISPLAY_FORMAT_FLOAT,*(E+i));
        if (i+1<N)
        printf (";");
    }
    printf ("]");

}

/*
 * Devuelve verdadero cuando los dos vectores son iguales (en los N primeros bits).
 */
int equal( neuron *vVector1, neuron *vVector2)
{
    int i=0;
    static int counter=0;
    //showMemory (vVector1);printf ("\n");
    //showMemory (vVector2);printf ("\n");

    for (i=0;i<N;i++) {
        if (vVector1[i]!=vVector2[i]) {
            //printf ("Son distintos en el %d-esimo bit",i);
            counter = 0;
            return 0;
        }
    }

    //printf ("Son iguales\n");
    return 1;
}

/*
 * Copia los primeros N bits del vector vVector2 en vVector1
 */
void copyMemory(neuron *vVector1, neuron *vVector2)
{
    int i;
    for (i=0;i<N;i++) {
        vVector1[i]=vVector2[i];
    }
}

/*
 * Actualiza la salida de la neurona i en la memoria Y, en base a la matriz
 * de pesos sinapticos.
 */
void evolve(float *W, float *Y, int i)
{
    float aux;
    float aux2;

    int j= 0;

    for(j=0;j<N;j++) {
        aux += ( (*(W+N*i+j)) * (Y[j]));
    }

    //printf ("Valor obtenido: (%f)\n",aux);
    //printf ("-Valor obtenido: (%i,%f)\n",N,aux);

    //tan h
    //aux2 = ( exp( aux ) - exp ( -aux ) ) / ( exp(aux) + exp( -aux) );
    aux2 = 1 / (1 + exp( -aux ));


    Y[i] = aux2;
}


/*
 * Realiza la evolucion de la red sobre Y en base a W, hasta que no se detecten
 * mas cambios sobre Y, y se hayan actualizado todas las neuronas.
 *
 * La actualizacion de las neuronas se realiza asincronicamente hasta que hallan pasado
 * REPLY_FACTOR iteraciones.
 *
 */
void evolveNet(float *W, neuron *Y)
{
    int iProb,i;
    neuron *Yaux;
    neuron *Neuron;
    neuron *I;
    long    counter=0;

    I = (neuron *)malloc(N*sizeof(neuron));
    Yaux = (neuron *)malloc(N*sizeof(neuron));
    Neuron = (neuron *)malloc(N*sizeof(neuron));

    // Se genera el vector Neuron donde estan las posiciones a actualizar.
    // Se genera el vector identidad que se utiliza para comparar.
    memset(Neuron, 0, N);
    for(i=0;i<N;i++)
        I[i]=1;

    while ( 1 )
    {
        copyMemory(Yaux, Y);
        iProb = (int)(N*1.0*rand()/(RAND_MAX+1.0));
        Neuron[iProb]=1;
        evolve(W, Y, iProb);
        //if ( equal(Yaux, Y) && equal(Neuron, I) )
        //    break;
        if (counter++ > REPLY_FACTOR) break;
    }

    free(Yaux);
    free(I);
    free(Neuron);
}


/*
 * Limpia la matriz de pesos sinapticos W.
 */
void clean( float *W )
{
    int i;
    for(i=0;i<N*N;i++)
	*(W+i) = 0;
}

void removeMemory(float *W, neuron *EE, int s, int d)
{
    int i,j;

    for(i=0;i<N;i++) {
        for(j=0;j<N;j++) {
            if (j!=i) {
                *(W+(i*N)+j)-=(*(EE+s*N+i))*(*(EE+d*N+j));
            }
        }
    }
}

/**
 * Agrega la memoria s correlacionada con la d
 *
 * W += Ss . Sd
 *
 */
void addMemory(float *W, neuron *EE, int s, int d)
{
    int i,j;

    for (i=0;i<N;i++) {
        for (j=0;j<N;j++) {
            if (j!=i) {
                *(W+(i*N)+j)+=(*(EE+s*N+i))*(*(EE+d*N+j));
            }
        }
    }
}

/*
 * Genera la matriz de pesos sinapticos en base a la matriz de datos a almacenar
 * EE.
 */
void getWeight(float *W, neuron *EE)
{
    // W es la matriz de pesos sinapticos.
    // EE es la matriz con los P datos a almacenar.
    int i;
    int j;
    int s;

    for (i=0;i<N;i++) {
        for (j=0; j<N; j++) {
            if (j==i) {
                *(W+(i*N)+j) = 0;
            } else {
                for (s=0;s<P;s++) {
                    *(W+(i*N)+j)+=(*(EE+s*N+i))*(*(EE+s*N+j));
                }
                *(W+(i*N)+j)*=(1.0/N);
            }
        }
    }

}


/*
 * Carga P memorias de N neuronas en EE de forma pseudoortogonal.
 */
void loadMemory( neuron *EE )
{
    int s;
    for (s=0;s<P;s++) {
        getMemory( EE + s*N );
    }
}

/*
 * Destruye una conexion entre dos neuronas que no se encuentre
 * actualmente desactivada ( wij=0 )
 *
 */
void alterOneWeight(float *W)
{
    int i;
    int j;
    int iCounter=0;

    while ( iCounter++ < 10 ) {
        i = getProb(0,N);
        j = getProb(0,N);
        if (*(W+i*N+j)!=0) {
            *(W+i*N+j)=0;
            break;
        }
    }
}

/*
 * Destruye iAlterNumber conexiones en la matriz de pesos sinapticos W.
 */
int alterWeight(float *W, int iAlterNumber)
{
    int iCounter;

    for (iCounter=0;iCounter<iAlterNumber;iCounter++) {
        alterOneWeight(W);
    }

    return (iCounter);
}

void initMemory(neuron **E)
{
    *E = (neuron *)malloc(N*sizeof(neuron));

    if (E==NULL) {
        printf ("Memoria insuficiente.\n");
        exit(-1);
    }

}

/*
 * Inicializa la memoria segun los parametros P y N.
 */
void init(float **W, neuron **EE,neuron **E, neuron **Y)
{
    *W = (float *)malloc(N*N*sizeof(float));
    *EE = (neuron *)malloc(P*N*sizeof(neuron));
    initMemory(E);
    initMemory(Y);

    if (W==NULL || EE==NULL || E==NULL || Y == NULL) {
        printf ("Memoria insuficiente.\n");
        exit(-1);
    }

    clean(*W);

}

/*
 * Inicializa una red de NxN tomando P memorias generadas al azar pseudoortogonales.
 */
void setUpNet(float *W, neuron *EE)
{
    loadMemory( EE );
    getWeight(W, EE);
}

