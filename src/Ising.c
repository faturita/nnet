/**
 *
 * Simulaci�n Proceso de Ising.
 *
 * Permite realizar una simulaci�n del proceso de Ising en 1 y 2 dimensiones
 * 
 * Fecha:	Mayo 
 *
 */
#include <stdio.h>
#include "mathNeuron.h"
#include "arrayView.h"

#define     TWODIM  N*N
#define     ONEDIM  N

#define     DIM     TWODIM

#define     INITIAL_T_VALUE     10
#define     MIN_T_VALUE         0.001
#define     DECREASE_T          0.01
#define     REPLY_FACTOR        100/0.01

// Cantidad de Elementos por Fila en el Ferromagneto
int N;

typedef float   energy;
typedef int     spin;

// Log
FILE *pfLogFile;
char msgBuffer[256];

/**
 * Inicializa el array de spines E
 *
 */
void initMagnet(spin **E)
{
    *E = (spin *)malloc(DIM*sizeof(spin));

    if (E==NULL) {
        printf ("Memoria insuficiente.\n");
        exit(-1);
    }
}


/**
 * Muestra el magneto en un arreglo de iRow x iCol
 *
 */
void showMagnet(spin *Y, int iRow, int iCol)
{
    int i,j;

    for (i=0;i<iRow;i++)
        for (j=0;j<iCol;j++) {
            if ( Y[i*iCol+j] == 1) {
                printf ("X");
            } else {
                printf (" ");
            }
            if (j+1<iCol) printf (" ");
            if (j+1==iCol) printf ("|\n");
    }

}

/**
 * Genera un array de spines de 1 y -1 pseudoaleatoriamente
 *
 */
void getMagnet(spin *E)
{
    int i;
    int iProb;

    for(i=0;i<DIM;i++) {
        iProb = 1+(int)(2.0*rand()/(RAND_MAX+1.0));
        if (iProb == 1)
            *(E+i) = 1;
        else
            *(E+i) = -1;
    }

}

/**
 * Devuelve la probabilidad de realizar un cambio de estado al pasar a un estado de mayor energia.
 *
 */
float getChangeProb(float DE, float k, float T)
{
    if (T==0)
        T = MIN_T_VALUE;
    if (DE <= 0)
        return 0;
    else {
        //return ( 1 -  exp( (-1*DE)/(k*T) ));
		return ( exp ( (-1*DE)/(k*T) ));
    }

}

/**
 * Altera el spin en la posicion iPos de Y
 */
int alterSpin(spin *Y, int iPos)
{
    Y[iPos]=Y[iPos]*(-1);
    return iPos;
}

/**
 * Altera el spin en una posicion pseudoaleatoria del array de spines Y
 */
int alterRandomSpin(spin *Y)
{
    int iPos = getProb(0,DIM);

    alterSpin(Y,iPos);

    return iPos;
}

/**
 * Disminuye la Temperatura T del sistema y devuelve el nuevo valor.
 *
 * La temperatura cae con un factor DECREASE_T
 *
 * Por ser T en � K el valor tiene que ser positivo.
 *
 **/
float decreaseTemp(float T)
{
    T -= (DECREASE_T);
    if (T<0)
        return MIN_T_VALUE;
    else
        return T;
}

/**
 * Obtiene el valor de magnetizacion de Y
 *
 **/
energy getMagnetization(spin *Y)
{
    energy M;
    int i;

    for (M=0,i=0;i<DIM;i++) {
        M += Y[i];
    }
    return M;
}

/**
 * Devuelve la energia del magneto
 *
 * Se calcula:
 *              E = -J * E(ij) Si Sj - H E(i) Si
 *
 * H:   campo aplicado
 * J:   constante de interaccion
 * Si:  spin del elemento i
 *
 * La interaccion de los elementos se da exclusivamente entre elementos adyacenter
 * (los separa solo una posicion en la el ferromagneto)
 *
 */
energy getEnergy(float J, float H, spin *Y)
{
    int i,j;
    energy E=0,E2=0;

    for (i=0;i<DIM;i++) {
        for (j=0;j<DIM;j++) {
            if ( (abs( i/N + i%N - j/N - j%N)==1) && abs(i/N-j/N)<=1 ) {
                // Los spines interactuan.entre si
                E += (Y[i]*Y[j]);
            }
        }
    }

    //printf ("%8.6f",E);printf ("\n");

    E *= ((-1)*J);
    //printf ("%8.6f",E);printf ("\n");

    for (i=0;i<DIM;i++) {
        E2 += Y[i];
    }
    //printf ("%8.6f",E2);printf ("\n");

    E2 *= ((-1)*H);

    E += E2;

    return E;
}

/**
 * Realiza la simulacion de Ising.
 *
 * Arranca la simulacion con una temperatura INITIAL_T_VALUE, que va dis
 * minuyendo con probabilidad 0.925.
 *
 * Cada simulacion se ejecuta hasta que el sistema alcanza un nivel de magnetizacion
 * del 90 % del total.
 *
 * 
 **/ 
float SimulateIsing()
{
    energy Ei;					// Energ�a inicial
    energy Ef;					// Energ�a final
    energy DE;					// Delta Energ�a
    energy M;					// Magnetizacion
    spin    *Y;					// Array de spines (Ferromagneto)
    float k=1;					// Constante de Boltzmann
    float T=INITIAL_T_VALUE;	// Temperatura inicial del sistema
    float H=1;					// Magnetizacion externa
    float J=1;					// Coeficiente de interaccion
    int iPos;					// Posicion del spin modificado
    int s=0,i=0;				// Indices
    float Pi,r;					// Probabilidades

	// Inicializar el algoritmo de generacion de numeros pseudoaleatorios.
    initRandom(0);

    N = 100;//En 2D es N*N

	printf ("Parametros del modelo:\n");
	printf ("Temperatura inicial:%8.6f\n",T);
	printf ("k=%8.6f, H=%8.6f,  J=%8.6f\n",k,H,J);
	printf ("Cantidad de elementos: %d \n",N*N);

	// Reserva espacio para el ferromagneto.
    initMagnet(&Y);

    // Mariz/Vector con los spines iniciales del sistema
    getMagnet(Y);showMagnet(Y,N,N);

	// Obtener la magnetizacion y energia inicial del sistema
    M = getMagnetization(Y);
	Ei = getEnergy(J,H,Y);

	// Informar por stdout
	printf ("Energia inicial del sistema:%8.2f\n",Ei);
	
    //while (s<REPLY_FACTOR) {
    while (M<=0.9*DIM && T>MIN_T_VALUE) {
		// Modificar el spin de un elemento
        iPos = alterRandomSpin(Y);

		// Energia Actual del sistema y delta energetica
        Ef = getEnergy(J,H,Y);
        DE = Ef - Ei;

        if ( DE > 0 ) {
            r = getNaturalProb();
            Pi = getChangeProb(DE,k,T);
            if (r<=Pi) {
                // Aceptar el cambio !
            } else {
                // Regresamos el spin al valor original
                alterSpin(Y,iPos);
            }
        }

        // Bajar la temperatura del sistema...
        if (getNaturalProb()>0.925) {
            T = decreaseTemp(T);
        } 

        // Obtener la magnetizacion total.
        M = getMagnetization(Y);

		// Log informacion de la iteracion
        sprintf (msgBuffer,"%8.6f,%8.6f,%8.6f\n",T,M,getEnergy(J,H,Y));
        fputs(msgBuffer, pfLogFile);
        fflush(pfLogFile);

        // Actualizar el valor de energia del sistema.
        Ei = getEnergy(J,H,Y);

    }

    printf ("Energia final del sistema:%8.2f\n",getEnergy(J,H,Y));

    showMagnet(Y,N,N);

	return T;
}

int main(int argc, char *argv[])
{
	int s=0;
	float Tc;
	float Tac=0;

	printf ("Simulacion Modelo de Ising (v1.0).\n");

	// Apertura de archivo de log
    if ((pfLogFile = fopen("ising.log","w+"))==NULL) {
        printf ("ERROR:  No es posible abir archivo de log.\n");
        exit(-1);
    }

	// Itera 10 veces promediando los valores obtenidos de Tc.
	for (s=0;s<10;s++) {
		Tc = SimulateIsing();

		sprintf (msgBuffer,"<Tc=%8.6f>\n",Tc);
		fputs(msgBuffer, pfLogFile);
		Tac+=Tc;
	}

	// Resultado de Temperatura Critica Obtenido.
    sprintf (msgBuffer,"Temperatura Critica Tc=%8.6f\n",Tac/s);
    fputs(msgBuffer, pfLogFile);

	fclose(pfLogFile);

    return 0;
}


