/**


Resolucion del problema del Travel Salesman Problem utilizando una red de Kohonen

La clave esta en que como la red de kohonen ordena los pesos eso se puede utilizar
para ir de una ciudad a otra con la menor distancia posible.

Elastic ring method es la forma de resolverlo, 

primero voy a empezar por un approach local, despues si no funca uso el del libro.

Son 500 ciudades a resolver....


Si tengo 500 ciudades, cada ciudad tiene 2 coordenadas...

Tengo 499x499 posibles viajes entre ciudades....

A su vez tengo 

**/

#include "Kohonen.c"



/**
 * Genera la matriz con los pesos sinapticos al azar para todos los layers
 *
 * Los pesos sinapticos se encuentran en el rango -1<Wi<1
 *
 * W				Matriz con los pesos sinapticos para todos los layers
 **/
void
getCircularWeight (weight ** W)
{
	int i, j, k;
	
	/**
	for (k = 0; k < D; k++)
	{
		for (i = 0; i < Di[k]; i++)
		{
			*(W[k] + Di[k + 1] * i + 0) = 1;
					//sin(i * ( (2.0*M_PI) / (float)Di[k] ) );
			*(W[k] + Di[k + 1] * i + 1) = 1;
					//cos(i * ( (2.0*M_PI) / (float)Di[k] ) );			
		}
	}
	**/
	
	for (i = 0; i < Di[1]; i++)
	{
		for (j = 0; j < Di[0]; j++)
		{
			*(W[0] + i * Di[0] + j) =sin(i * ( (2.0*M_PI) / (float)Di[k] ) );
		}
	}
	

	return;
}


void
learnPatterns2 (weight ** W, neuron ** E, neuron ** Y, int patternSize)
{
	int i, j, s;
	int winner;
	int replyCounter = 0;
	int replyFactor;
	char buffer[MAXSIZE];

	getValue (buffer, "reply.factor", "kohonentsp.conf");
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
			if ((s != winner) && ( (abs (s - winner) <= 10)
			    || (abs (s + winner - Di[1]) <= 10) ) )
			{
				//printf ("Actualizando %d,%d \n", XX(s), YY(s));
				for (i = 0; i < Di[0]; i++)
				{
					*(W[0] + s * Di[0] + i) +=
						((1.0 / (100.0 * abs(s - winner))) *
						 (E[0][i] -
						  *(W[0] + s * Di[0] + i)));
				}
			}

		}

	}
}



void
generateTrainningSet (int patternSize)
{
	neuron *item;
	neuron *zero;
	int i, j;


	printf ("pattern.size=%d\n\n", patternSize);

	item = (neuron *) malloc (sizeof (neuron) * Di[0]);
	zero = (neuron *) malloc (sizeof (neuron) * Di[0]);

	memset (zero, 0, sizeof (neuron) * Di[0]);

	for (i = 0; i < patternSize;)
	{
		for (j = 0; j < Di[0]; j++)
		{
			item[j] = getNaturalMinMaxProb (-1, 1);
		}
		if (getEuclideanNorm (zero, item, Di[0]) <= 1)
		{
			for (j = 0; j < Di[0]; j++)
			{
				printf ("pattern.in.%d.%d=%12.10f\n", i, j,
					item[j]);
			}
			i++;
		}
	}

	free (item);
	free (zero);
}

int
main_Generate (int argc, char *argv[])
{
	initRandom ();
	config ("kohonentsp.conf");

	generateTrainningSet (atoi (argv[1]));

	return 1;
}

int
main (int argc, char *argv[])
{
	weight **W;
	neuron **E;
	neuron **X, **Y;
	int patternSize;
	char buffer[256];

	int i, j,iChance;
	int winner;

	initRandom ();

	config ("kohonentsp.conf");

	init (&W, &E);
	initLearningPatterns (&X, &Y, "kohonentsp.pattern.conf");
	getLearningPatterns (X, Y, "kohonentsp.pattern.conf");

	//getRandomWeight (W);
	getCircularWeight(W);

	//getValue (buffer, "pattern.size", "kohonentsp.pattern.conf");
	//patternSize = atoi (buffer);

	// Aplica el algoritmo de aprendizaje a los patrones de entrada.
	//learnPatterns2 (W, E, X, patternSize);

	// Imprime la matriz de resultado.
	printf ("pattern.size=%d\n\n", Di[1]);
	for (i = 0; i < Di[1]; i++)
	{
		for (j = 0; j < Di[0]; j++)
		{
			printf ("pattern.in.%d.%d=%12.10f\n", i, j,
				*(W[0] + i * Di[0] + j));
		}
	}

	return 1;
}
