/**

La estrctura de la red de Kohonen:

Una capa sola, que tendra N entradas y M salidas.
 N entradas valuadas (-1,1)
 M salidas valuadas  (0,1)

 Hay p patrones para entrenar la red de los cuales no se sabe cual es la salida.

 Algoritmo:
 - Armar la red utilizando los parametros de configuracion.
 - Generar al azar los valores de los pesos sinapticos.
 - Entrenar la red con los P patrones de entrada.
   - Para cada patron, escoger el peso que tenga menor distancia euclideana.
   - Actualizar todas las neuronas alrededor, segun Nc, del ganador segun winner-take-all 
 - Hacer esto hasta que los pesos sinapticos conformen un mapa topografico.
 - Escoger un nuevo patron, y elegir el peso sinaptico con menor distancia.  Este va a ser el clasificador.


 Que se necesita ?
 - Armar la red utilizando los parametros.
 - Generar las funciones para hacer evolucionar la red.
 - Generar las funciones de aprendizaje.
 - Armar un programa para "ver" el mapa topografico.
 - Variar los parametros Nc y Alfa de aprendizaje.

 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "mathNeuron.h"
#include "arrayView.h"
#include "parameter.h"
#include "logger.h"


FILE *pfLogFile;

// Tipo de dato para las salidas y entradas de las neuronas
typedef float neuron;
// Tipo de dato para los pesos sinapticos
typedef float weight;

// Cantidad de layers para el perceptron (camino maximo del grafo)
int D;

// Array con la cantidad de neuronas en cada layer. Dim(Di) = D+1
// Para Di[0]=Di[0] real + 1 (bias con entrada fija -1)
int *Di;




/**
 * init
 *
 * Inicializacion de las estructuras de datos para simular la red.
 *
 * W				Puntero a la matriz W.
 * E				Puntero a las entradas y salidas de toda la red.
 **/
void
init (weight *** W, neuron *** E)
{
	int i = 0;

	*W = (weight **) malloc (sizeof (weight *) * D);
	*E = (neuron **) malloc (sizeof (weight *) * (D + 1));

	if (*W == NULL || *E == NULL)
	{
		printf ("No hay suficiente memoria.\n");
		exit (-1);
	}

	// Inicializacion de los vectores de salidas de cada capa.
	for (i = 0; i < (D + 1); i++)
	{
		(*E)[i] = (neuron *) malloc (sizeof (neuron) * Di[i]);
		if ((*E)[i] == NULL)
		{
			printf ("No hay suficiente memoria.\n");
			exit (-1);
		}
	}

	// Inicializacion matriz con los pesos sinapticos para todas las capas.
	for (i = 0; i < D; i++)
	{
		(*W)[i] =
			(weight *) malloc (sizeof (weight) *
					   (Di[i] * Di[i + 1]));
		if ((*W)[i] == NULL)
		{
			printf ("No hay suficiente memoria.\n");
			exit (-1);
		}
	}

}

/**
 * Inicializa las estructuras para contener a los patrones para
 * entrenar la red.
 *
 * X				Puntero a los vectores de entrada de la red.
 * Y				Puntero a los vectores de salida de la red.
 * filename		Nombre del archivo donde estan configurados los patrones
 *
 **/
void
initLearningPatterns (neuron *** X, neuron *** Y, char *filename)
{
	int iSize;
	int p;
	char buffer[MAXSIZE];

	// Parametro de cantidad de patrones.
	getValue (buffer, "pattern.size", filename);
	iSize = atoi (buffer);

	*X = (neuron **) malloc (sizeof (neuron *) * iSize);
	*Y = (neuron **) malloc (sizeof (neuron *) * iSize);

	if (((*X) == NULL) || ((*Y) == NULL))
	{
		printf ("No hay suficiente memoria.\n");
		exit (-1);
	}

	for (p = 0; p < iSize; p++)
	{
		*(*X + p) = (neuron *) malloc (sizeof (neuron) * Di[0] - 1);	// -1 por el bias
		*(*Y + p) = (neuron *) malloc (sizeof (neuron) * Di[D]);

		if (((*X + p) == NULL) || (*(*Y + p) == NULL))
		{
			printf ("No hay suficiente memoria.\n");
			exit (-1);
		}
	}
}

/**
 * Obtiene del archivo de configuracion los patrones de entrada y
 * salida correspondientes para entrenar la red.
 *
 * X					Vectores de entrada.
 * Y					Vectores de salida.
 *
 * filename			Nombre del archivo donde se encuentran los patrones.
 *
 **/
void
getLearningPatterns (neuron ** X, neuron ** Y, char *filename)
{
	int iSize;
	int p;
	int j;
	char buffer[MAXSIZE];
	char aux1[MAXSIZE];

	getValue (buffer, "pattern.size", filename);
	iSize = atoi (buffer);

	for (p = 0; p < iSize; p++)
	{
		for (j = 0; j < Di[0]; j++)
		{
			sprintf (aux1, "pattern.in.%d.%d", p, j);
            getQuickValue (buffer, aux1, filename,"0");
			// TODO: Verificar los tipos de datos segun el problema a resolver
			//X[p][j]=atoi(buffer);
			X[p][j] = (neuron) atof (buffer);
		}

      /**for(j=0;j<Di[D];j++) {
	   	sprintf(aux1,"pattern.out.%d.%d",p,j);
         getQuickValue(buffer,aux1,filename);
         // TODO: Verificar los tipos de datos segun el problema a resolver.
			//Y[p][j]=atoi(buffer);
         Y[p][j]=(neuron)atof(buffer);
      } */

	}
}

/**
 * Configura las estructuras globales que definene la topologia de la red
 * multicapas.
 *
 * filename				Nombre del archivo con los parametros de la red.
 *
 **/
int
config (char *filename)
{
	char buffer[MAXSIZE];
	char aux1[MAXSIZE];
	char aux2[MAXSIZE];
	int k;

	getValue (buffer, "layer.size", filename);
	D = atoi (buffer);

	// +1 por las entradas
	Di = (int *) malloc (sizeof (int) * (D + 1));

	if (Di == NULL)
	{
		printf ("No hay suficiente memoria.\n");
		exit (-1);
	}

	for (k = 0; k < D + 1; k++)
	{
		sprintf (aux1, "layer.%d", k);
		getValue (buffer, aux1, filename);
		Di[k] = atoi (buffer);
		// +1 por el bias.
		//if (k==0) Di[k]++;
	}

    printf("Done\n");

	// TODO: Asserts for null pointers
	// TODO: Check if there is a misconfiguration file.
	return 1;
}

/**
 * Genera la matriz con los pesos sinapticos al azar para todos los layers
 *
 * Los pesos sinapticos se encuentran en el rango -1<Wi<1
 *
 * W				Matriz con los pesos sinapticos para todos los layers
 **/
void
getRandomWeight (weight ** W)
{
	int i, j, k;

	for (k = 0; k < D; k++)
	{
		for (i = 0; i < Di[k]; i++)
		{
			for (j = 0; j < Di[k + 1]; j++)
			{
                *(W[k] + Di[k + 1] * i + j) =  0.5*cos(M_PI);
					getNaturalMinMaxProb (-1, 1);
			}
		}
	}

	return;
}

/*
 * Actualiza la salida de la neurona i en la memoria Y, en base a la matriz
 * de pesos sinapticos.
 *
 * X				Vector de entrada para la nuerona
 * W				Matriz de pesos sinapticos.
 * i				Neurona en actualizacion
 * jMax			Cantidad de entradas para la nuerona
 */
neuron
evolve (neuron * X, weight * W, int i, int jMax)
{
	weight aux = 0;
	int j = 0;

	for (j = 0; j < jMax; j++)
	{
		aux += ((*(W + jMax * i + j)) * X[j]);
	}

	// TODO: Generalize this
	//return sgn(aux);
	//return expsigmoid(aux);
	return tanhsigmoid (aux);
}

/**
 * Evolve the layered network, layer by layer returning a pointer
 * to the final output.
 *
 * W            Layered Weight Matrix
 * E            Layered IO's
 * return       Pointer to the final output of E
 */
neuron *
evolveLayeredNN (weight ** W, neuron ** E)
{
	int k;			// Indice de layer
	int i;			// Indice de neurona en cada layer

	// El primer layer tiene las entradas, asi que empezamos del segundo
	// para procesar hacia delante.
	for (k = 0 + 1; k < D + 1; k++)
	{
		for (i = 0; i < Di[k]; i++)
		{
			E[k][i] = evolve (E[k - 1], W[k - 1], i, Di[k - 1]);
		}
	}

	return E[D];
}


/**
 * Devuelve el error cuadratico medio para una red neuronal feed forward multicapas
 * en base a la comparacion salida contra salida deseada (Y) promediado
 * sobre los patternSize patrones.
 *
 * W				Matriz pesos sinapticos para todos los layers
 * E				Valores de salida para cada neurona en todos los layers
 * X				Entradas
 * Y 				Salidas
 * patternSize	Cantidad de patrones
 *
 * Este calculo solo tiene validez cuando la red es single-output.
 **/
	float
getQuadraticError (float **W, float **E, float **X, float **Y,
		   int patternSize)
{
	int p, j;
	float fQErr;

	for (p = 0; p < patternSize; p++)
	{
		E[0][0] = -1;
		for (j = 1; j < Di[0]; j++)
		{
			E[0][j] = X[p][j - 1];
		}
		evolveLayeredNN (W, E);
		fQErr += (Y[p][0] - E[D][0]) * (Y[p][0] - E[D][0]);
	}
	fQErr = fQErr / (float) patternSize;
	return fQErr;
}

void
logQuadraticError (weight ** W, neuron ** E, neuron ** X, neuron ** Y,
		   int patternSize)
{
	if (isLogging ())
	{
		sprintf (logBuffer, "%12.10f\n",
			 getQuadraticError (W, E, X, Y, patternSize));
		logInfo (logBuffer);
	}
}

neuron
getEuclideanNorm (weight * E, weight * W, int jMax)
{
	weight aux = 0;
	int j;

	for (j = 0; j < jMax; j++)
	{
		aux += (E[j] - W[j]) * (E[j] - W[j]);
	}

	return sqrt (aux);
}

int
evolveSimilarityMatching (weight ** W, neuron ** E)
{
	weight minNorm, norm;
	int j, minNeuron;
	int i;

	minNorm = getEuclideanNorm (E[0], (W[0] + 0), Di[0]);
	minNeuron = 0;

	for (i = 0; i < Di[D]; i++)
	{
		norm = getEuclideanNorm (E[0], (W[0] + i * Di[0]), Di[0]);
		if (norm < minNorm)
		{
			minNeuron = i;
			minNorm = norm;
		}
	}

	// Todas en cero, excepto la ganadora.
	for (j = 0; j < Di[D]; j++)
	{
		E[D][j] = 0;
	}
	E[D][minNeuron] = 1.0;	// Full activated.

	return minNeuron;

}

int
XX (int s)
{
	return s / 13;
}

int
YY (int s)
{
	return s % 13;
}

void
learnPatterns (weight ** W, neuron ** E, neuron ** Y, int patternSize)
{
	int i, j, s;
	int winner;
	int replyCounter = 0;
	int replyFactor;
	char buffer[MAXSIZE];

	getValue (buffer, "reply.factor", "kohonen.conf");
	replyFactor = atoi (buffer);

	//E[0][0]=-1;
	while (replyCounter++ < replyFactor)
	{

		// Seleccion de uno de los patrones al azar.
		s = getProb (0, patternSize);

		// Copia del patron en la entrada a la red.
		for (i = 0; i < Di[0]; i++)
		{
			E[0][i] = Y[s][i];
		}

		// Se selecciona el ganador con "Similarity Matching"
		winner = evolveSimilarityMatching (W, E);
		//printf ("Winner: %d\n", winner);
		// update synaptic weights  Di[0] dimension del vector de entrada.

		//printf ("La ganadora es %d,%d \n", XX(winner), YY(winner));
		// Actualizacion de los pesos sinapticos de la neurona ganadora
		// "acercandolos" a los del valor de entrada.
		for (i = 0; i < Di[0]; i++)
		{
			*(W[0] + winner * Di[0] + i) +=
				(0.1 *
				 (E[0][i] - *(W[0] + winner * Di[0] + i)));
		}

		// Winner Take all para los vecinos. (Hardcoded neuronas en 2 dimensiones).
		for (s = 0; s < Di[1]; s++)
		{
			if ((s != winner) && (abs (XX (s) - XX (winner)) <= 1)
			    && (abs (YY (s) - YY (winner)) <= 1))
			{
				//printf ("Actualizando %d,%d \n", XX(s), YY(s));
				for (i = 0; i < Di[0]; i++)
				{
					*(W[0] + s * Di[0] + i) +=
						(0.01 *
						 (E[0][i] -
						  *(W[0] + s * Di[0] + i)));
				}
			}

		}

	}
}

