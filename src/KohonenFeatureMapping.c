#include "Kohonen.c"

/**
 * Genera los patrones al azar que son muestras pertenecientes al circulo unitario.
 *
 ***/
void
generateTrainningSet (int patternSize)
{
	neuron *item;
    neuron *zero, *one, *two;
	int i, j;


	printf ("pattern.size=%d\n\n", patternSize);

	item = (neuron *) malloc (sizeof (neuron) * Di[0]);
	zero = (neuron *) malloc (sizeof (neuron) * Di[0]);
    memset (zero, 0, sizeof (neuron) * Di[0]);
    one = (neuron *) malloc(sizeof (neuron) * Di[0]);
    memset (one, 0, sizeof (neuron) * Di[0]);
    two = (neuron *) malloc(sizeof (neuron) * Di[0]);
    memset (two, 0, sizeof (neuron) * Di[0]);

    for(j=0;j<Di[0];j++) one[j] = -0.6;
    for(j=0;j<Di[0];j++) two[j] = +0.6;


	for (i = 0; i < patternSize;)
	{
		for (j = 0; j < Di[0]; j++)
		{
			item[j] = getNaturalMinMaxProb (-1, 1);
		}
        if ((getEuclideanNorm (zero, item, Di[0]) <= 0.2)  ||
            (getEuclideanNorm (one,  item, Di[0]) <= 0.2)  ||
            (getEuclideanNorm (two,  item, Di[0]) <= 0.2) )
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

    printf ("Kohonen\n");
    //signal(SIGINT, sigintHandler);

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
        generateTrainningSet(atoi (argv[3]));
        exit (0);
    }

    getValue (patternFilename, "pattern.filename", argv[1]);


    init (&W, &E);
    initLearningPatterns (&X, &Y, patternFilename);
    getLearningPatterns (X, Y, patternFilename);

    getRandomWeight (W);

    getValue (buffer, "pattern.size", patternFilename);
	patternSize = atoi (buffer);


    printf("Learning....\n");

    learnPatterns (W, E, X, patternSize);
	//showWeight(W[0],Di[1],Di[0]);

	//showRNeuron(E[0], Di[0]);
	
	// Graba la salida para la graficacion del mapeo en el circulo unitario.
	// La salida la saca por pantalla y contiene los valores de los pesos sinapticos.
	printf ("pattern.size=%d\n\n", Di[1]);
	for (i = 0; i < Di[1]; i++)
	{
		for (j = 0; j < Di[0]; j++)
		{
			printf ("pattern.in.%d.%d=%12.10f\n", i, j,
				*(W[0] + i * Di[0] + j));
		}
	}
	
	// Test data learned
	iChance = getProb (0, patternSize);

	for (i = 0; i < Di[0]; i++)
	{
		E[0][i] = X[iChance][i];
	}
	

	winner = evolveSimilarityMatching (W, E);

	//showRNeuron(E[1],Di[1]);
	printf ("Winner neuron is %d:\n", winner);

	// Test data

	return 1;
}
