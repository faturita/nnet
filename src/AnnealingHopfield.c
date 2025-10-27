/**
 * Implementaci�n de una red de hopfield 82, pero utilizando para la soluci�n del aprendizaje de los patrones
 * Simulated Annealing......
 * Tenemos primero que definir el funcional que para este caso es la funci�n de Energ�a de H'82
 * 
 * E = -1/2 Ei Ej  Uij(t) Yi Yj - 1/2 Ei Ej Uij(t) Y2i Y2j
 * 
 * Con este funcional, el sistema no funciona ya que el mismo no se encuentra acotado, no tiene un minimo.
 *
 *
 * Por lo tanto se cambia el funcional por este:
 *
 * SUMk  ||  Yk  -  W Yk || ^ 2  
 *
 * 
 *
 * Arranco con T en 100 y desciende en funci�n t(t+1) = b T(t), siendo b el coeficiente de variaci�n
 * Uij (t+1) = Nu  p  Uij(t), Siendo N el coeficiente de variaci�n de los pesos sin�pticos, p azar <-1,1>
 * 
 * k, contaste de Boltzman = 1
 * Pseudocodigo:
 *
 * - Genero un Uij simetr�co al azar.
 * A
 * - Calculo la Energ�a del Sistema Et = E = -1/2 Ei Ej  Uij(t) Yi Yj - 1/2 Ei Ej Uij(t) Y2i Y2j
 * - Elijo un valor de i y de j al azar y de p, entre 1.-1, y actualizo Uij (t+1) = Nu p Uij(t)
 * - Calculo la Energia del Sistema Et+1
 * - DE = Et+1 - Et
 * - Si DE >= 0 
 * - Calculo r random (0,1); Si r < P=exp (  -DE / (T*k) )     ----->    Acepto
 * - Sino rechazo la variaci�n de Uij y Uij(t+1)=Uij(t)
 * - Si DE < 0 continuo
 * - Var�o T seg�n g(T,t)=Tt+1= b T(t)
 * - Si la DE es cero en REPLY_FACTOR iteraciones y la T < Tc cierta temperatura cr�tica del sistema, termina.
 * - Sino Continua.
 *
 * -- Al terminar en Uij deber�an estar los pesos sinapticos para recordar los dos patrones almacenadso
 *
 *
 **/

#include <stdio.h>
#include <stdlib.h>
#include "gridParser.h"
#include "mathNeuron.h"
#include "neuron.c"

#define     MIN_T_VALUE         0.001

/**
 * Devuelve la probabilidad de realizar un cambio de estado al pasar a un estado de mayor energia.
 * Tiene que estar en una libreria externa.
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
 * Calcula la energia del sistema.
 *
 *
 **/
float getAnnealingHopfieldEnergy(weight *W, int *EE) {
	int i,j,k;
	float S[N],SS=0;

	for (k=0;k<P;k++) {
		for (i=0;i<N;i++) {
			S[i] = 0;
			for (j=0;j<N;j++) {			
				S[i] += (W[i*N+j] * EE[k*N+j]);
			}
		
			S[i] = EE[k*N+i] - S[i];
		
		}
	
		for (i=0;i<N;i++) {
			S[i] = S[i] * S[i];
		
			SS += S[i];
		
		}
	}
	
	return SS;
}




/** 
 * Se actualiza simetricamente la matriz. 
 **/
void getRandomWeightMatrix( weight *W ) {
	int i=0,j=0;
	for(i=0;i<N;i++) {
		for (j=0;j<N;j++) {
			if (i<j) {
				W[i*N+j] = getNaturalMinMaxProb(-1,1);
				W[j*N+i] = W[i*N+j];
			} else if (i==j) {
				W[i*N+j] = 0;
			}
		}
	}
}

/**
 * Varia Nu en la direccion de p el valor del peso sinaptico (i,j)
 *
 * La actualizacion es simetrica.
 *
 **/
weight updateWeightMatrix( weight *W, float Nu, int p, int i, int j) {
	weight w = W[i*N+j];
	
	W[i*N+j] += (Nu * p);
	
	// Acoto los valores de los pesos sinapticos entre -1 < Wij < 1
	if (W[i*N+j] < -1) 
		W[i*N+j] = -1;
	if (W[i*N+j] > 1)
		W[i*N+j] = 1;
	
	// Actualizo la sinapsis simetrica.
	W[j*N+i] = W[i*N+j];
	
	return w;
}


/**
 * 
 * Simulated Annealing para la red de Hopfield 82.
 *
 * Permite aprender los patrones en EE, variando los pesos sinapticos minimizando un funcional
 * asociado a la red de Hopfield.
 *
 *
 **/
void AnnealingHopfield(weight *W, int *EE, int MIN_DE)
{
	float T=100;			// Temperatura del modelo.
	
    initRandom(0);
	
	float Et0,Et1,DE,LREt0=0;
	int i,j;
	int p;
	float r,Pcambio;
	
	int replyFactor=0;
	
	float Nu = 0.0001;
	float b = 0.99;
	float k = 1;
	
	int DE_negative=0,it=0;
	
	// Se inicializa la matriz con los pesos sinapticos en base a N
	getRandomWeightMatrix(W);
	//getWeight(W,EE);
	
	showWeight(W,N,N);printf ("\n\n");
	
	// Arranco con una variacion imporante.
	Nu = 0.1;
	

	do {
	
		Et0 = getAnnealingHopfieldEnergy( W, EE );

		// genero i, j al azar, i de 0 a N-1 y j de 0 a N-1
		do {
			i = getProb(0,N);
			j = getProb(0,N);
		} while ( i==0 && j== 0);
		
		//printf ("Actualizando (%d,%d)\n",i,j);
		
		// Elijo p que va a indicar el signo de la actualizacion.
		p = getSpinProb(); // discreto
		
		// Salvo el valor original
		if (Et0 < 10) { 
			if (Nu == 0.1) 
			{ 
				Nu = Nu / 10;
			}
		}
		if (Et0 < 5) {
			if (Nu == 0.001) {
				Nu = Nu / 10;
			}
		}
		
		weight w = updateWeightMatrix( W, Nu, p, i, j);
		
		// Recalculo el valor de energia del sistema ahora.
		Et1 = getAnnealingHopfieldEnergy( W, EE);
		
		float DE = Et1 - Et0;
		
		// Si la variacion de energia es positiva con probabilidad P(cambio) acepto el nuevo peso.
		// Si la energia del sistema es menor se esta logrando lo que se quiere asi que se continua.
		
		if (DE > 0) {
			r = getNaturalProb();
			Pcambio = getChangeProb(DE,k,T) ;
			
			if (r >= Pcambio || fabs(DE)>0.0000001 || T < 0.00001 ) { 
				// Rechazo, se vuelve atras la modificacion del peso sinaptico.
				
				W[i*N+j] = w;
				W[j*N+i] = w;
				
				//printf ("+xE=%14.10f\tDE=%14.10f\tT=%14.10f\t\tR=%14.10f\t%14.10f\n",Et1,DE,T,r,Pcambio);
				
				
			} else {
				printf ("+ E=%14.10f\tDE=%14.10f\tT=%14.10f\t\tR=%14.10f\t%14.10f\n",Et1,DE,T,r,Pcambio);
			}
			
		} else if (DE < 0) {
			//printf ("- E=%14.10f\tDE=%14.10f\tT=%14.10f\t\tX           \tX\n",Et1,DE,T,r,Pcambio);
			DE_negative++;
		} else {
			//printf ("= E=%14.10f\tDE=%14.10f\tT=%14.10f\t\tX           \tX\n",Et1,DE,T,r,Pcambio);
		}
		it++;
		if (it == 100000) {
			it=0;
			printf ("= E=%14.10f\tDE=%14.10f\tT=%14.10f\t\tR=%14.10f\t%14.10f\n",Et0,DE,T,r,Pcambio);
			printf ("Decrementos:%d\n", DE_negative);
			DE_negative=0;
			if (T < 0.000001 ) { 
				printf ("Temperature too low....\n");
				break;
			}
			if (Et0 < 0.1 ) { 
				printf ("Energy is low enough...\n");
				break;
			}
			
			/***if (LREt0 == Et0) {
				printf ("No se producen cambios de energia...\n");
				// genero i, j al azar, i de 0 a N-1 y j de 0 a N-1
				it = 0;
				for(it=0;it<10;it++) { 
				do {
					i = getProb(0,N);
					j = getProb(0,N);
				} while ( i==0 && j== 0);
				
				//printf ("Actualizando (%d,%d)\n",i,j);
				
				W[i*N+j] = getNaturalMinMaxProb(-1,1);
				
				W[j*N+i] = W[i*N+j];
				}
				
				//break;
				
				
				
			}***/
			LREt0 = Et0;
			
		}
		// Ojo que al comenzar T puede ser muy grande y b muy chico...
		if (replyFactor++ > (10/(T*100)) ) {
			T = b * T;
			replyFactor=0;
		}
	} while ( 1 );
	
}


int main(int argc, char*argv[]) {
	char sFileGridConf[] = "annealing.grid.data.conf";
	char sFileGridIn[]   = "annealing.grid.in";
	
	printf ("Simulacion Red Hopfield 82 utilizando Simulated Annealing (v1.0).\n");

    initRandom (0);

	getParameterWithFileName ("annealing.parameter.conf", &N, &P, &LC, &N_MIN, &N_MAX, &N_INTERVAL);
	
	printf ("Parametros :\n");
	printf ("N - Cantidad de neuronas: %d\n",N);
	printf ("P - Cantidad de patrones: %d\n",P);
	
	float *W;		// Matriz de pesos sinapticos
	int *E;			// Dato auxiliar.
	int *Y;			// Salida de la red.
	int *EE;		// Matriz de datos a almacenar
	int s;

	// Inicializa la estructura de las matrices.
	init (&W, &EE, &E, &Y);

	// Se graban los patrones a memorizar.
	for (s = 0; s < P; s++)
	{
		Y = getGridMemory (sFileGridConf, s);
		copyMemory (EE + s * N, Y);
	}

	if ((argc>1) && (strcmp(argv[1],"-h")==0)) {
		// Se genera la matriz con los pesos sinapticos (aprendizaje de Hopfield);
		getWeight (W, EE);    ////Aprendizaje Hebbiano.
	
		printf ("Energia de Hebbian Hopfield : %14.10f \n",getAnnealingHopfieldEnergy( W, EE));
	} 
	if ((argc>1) && (strcmp(argv[1],"-l")==0)) {
		loadWeight("annealinghopfield.weight",W);
		
		printf ("Energia %14.10f \n",getAnnealingHopfieldEnergy( W, EE));
	}
	
	if ((argc>3) && (strcmp(argv[2],"-a")==0)) {
		AnnealingHopfield(W, EE, atoi(argv[3]));
	
		printf ("Energia de Hebbian Simm Ann: %14.10f \n",getAnnealingHopfieldEnergy( W, EE));
	}
	
	
	saveWeight("annealinghopfield.weight",W);

	// Se carga el patron a verificar.
	E = getGridMemory (sFileGridIn, 0);

	// Se hace evolucionar la red, con lo que se modifica el dato ingresado.
	evolveNet (W, E);

	//showMemory(E);
	showGridMemory (E);	
	
}



