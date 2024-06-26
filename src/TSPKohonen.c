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
#include "commandline.h"

extern char logBuffer[256];

int forceBreak = 0;

void sigintHandler(int sig_num)
{
    /* Reset handler to catch SIGINT next time.
       Refer http://en.cppreference.com/w/c/program/signal */
    signal(SIGINT, sigintHandler);
    forceBreak = 1;
    fflush(stdout);
}

/**
 * Genera la matriz con los pesos sinapticos al azar para todos los layers
 *
 * Los pesos sinapticos se encuentran en el rango -1<Wi<1
 *
 * W				Matriz con los pesos sinapticos para todos los layers
 *
 * @NOTE: las capas en las estructuras de datos de la red de kohonen están invertidas.
 **/
void
getCircularWeight (weight ** W)
{
	int i, j, k;

    for (i = 0; i < Di[1]; i++)
    {
        for (j = 0; j < Di[0]-1; j++)
        {

            *(W[0] + i * Di[0] + j) = 0.5 * sin( (float)i * 2.0 * M_PI / Di[1] );

            *(W[0] + i * Di[0] + j + 1) = 0.5 * cos ( (float)i * 2.0 *  M_PI / Di[1]);


        }
        //printf("%12.10f,%12.10f\n", *(W[0] + i * Di[0] + 0),*(W[0] + i * Di[0] + 1));
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
    while (replyCounter++ < replyFactor && forceBreak==0)
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
        //printf ("Winner: %d - %d\n", s,winner);
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

//					*(W[0] + s * Di[0] + i) +=
//                        0.01 *
//						 (E[0][i] -
//                          *(W[0] + s * Di[0] + i));
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
                //printf ("pattern.in.%d.%d=%12.10f\n", i, j,
                    //item[j]);
			}
            printf ("%12.10f,%12.10f\n", item[0], item[1]);
			i++;
		}
	}

	free (item);
	free (zero);
}



int
main (int argc, char *argv[])
{
	weight **W;
	neuron **E;
	neuron **X, **Y;
	int patternSize;
	char buffer[256];
    char patternFilename[256];

	int i, j,iChance;
	int winner;

    unsigned int timeseed=0;

    printf ("TSP Kohonen\n");
    signal(SIGINT, sigintHandler);

    if (argc < 2)
    {
        printf ("\nSinopsis:\n");
        printf ("%s    [archivo de configuracion inicial]\n",
            argv[0]);
        printf ("\n-f 	Genera archivo de configuracion con las inmagenes.\n");
        exit (-1);
    }

    // Configuracion de la red
    config (argv[1]);

    // Inicializacion de la generacion de numeros pseudoaleatorios
    timeseed = initRandom (timeseed);

    if ((argc >2 && strcmp (argv[2], "-f") == 0))
    {
        // Auxiliar: Ejecucion de la generacion de los patrones para aprender
        // una funcion especifica.
        generateTrainningSet (atoi (argv[3]));
        exit (0);
    }

    getValue (patternFilename, "pattern.filename", argv[1]);

	init (&W, &E);
    initLearningPatterns (&X, &Y, patternFilename);
    getLearningPatterns (X, Y, patternFilename);


    // Inicializa los pesos sinapticos formando un circulo en el centro del "espacio".
    getCircularWeight(W);

    getValue (buffer, "pattern.size", patternFilename);
    patternSize = atoi (buffer);

    if (!isPresentCommandLineParameter(argc,argv,"-nolearn"))
    {
        // Aplica el algoritmo de aprendizaje a los patrones de entrada.
        learnPatterns2 (W, E, X, patternSize);
    }

	// Imprime la matriz de resultado.
	printf ("pattern.size=%d\n\n", Di[1]);
	for (i = 0; i < Di[1]; i++)
	{
		for (j = 0; j < Di[0]; j++)
		{
            printf ("pattern.in.%d.%d=%12.10f\n", i, j,
                *(W[0] + i * Di[0] + j));


		}
        //printf("%12.10f,%12.10f\n", *(W[0] + i * Di[0] + 0),*(W[0] + i * Di[0] + 1));
	}

	return 1;
}
