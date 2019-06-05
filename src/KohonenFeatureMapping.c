#include "Kohonen.c"

/**
 * Genera los patrones al azar que son muesras pertenecientes al circulo unitario.
 *
 ***/
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
	config ("kohonen.conf");

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

	config ("kohonen.conf");

	init (&W, &E);
	initLearningPatterns (&X, &Y, "kohonen.pattern.conf");
	getLearningPatterns (X, Y, "kohonen.pattern.conf");

	getRandomWeight (W);

	getValue (buffer, "pattern.size", "kohonen.pattern.conf");
	patternSize = atoi (buffer);

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