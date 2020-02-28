/**
 * Implementacion de un perceptron multicapas.
 *
 * Obtiene los parametros de archivos de configuracion, uno global para los
 * parametros de la red, y otro con los patrones que la red debe aprender.
 *
 * Una vez aprendidos los patrones (cuando converge el algoritmo de backpropagation)
 * se prueban todos los patrones del trainning set y luego se generan patrones
 * al azar y se prueban contra la red verficando la generalizacion.
 *
 * 
 **/
#include "layeredNNet.c"

FILE *pfLogFile;
FILE *pfInput;
FILE *pfOutput;


int
main (int argc, char *argv[])
{
	weight **W;		// Matriz de pesos sinapticos para toda la red
	neuron **E;		// Salidas de todas las neuronas de la red
	neuron **Y;		// Valores deseados para las salidas finales de la red
	neuron **X;		// Patrones de entrada (Trainning Set)

	char buffer[MAXSIZE];	// Buffer auxiliar
	char patternFilename[MAXSIZE];	// Nombre del archivo de patrones.
	char logFilename[MAXSIZE];	// Nombre del archivo de logging.
	int patternSize;	// Cantidad de patrones
	int i, s;		// Contadores auxiliares
	int bShowOutputFx = 0;	// Marca para mostrar el valor de la funcion en la salida.

	// Inicializacion de la generacion de numeros pseudoaleatorios
	initRandom ();

	printf ("Backpropagation\n");

	if (argc != 2)
	{
		printf ("\nSinopsis:\n");
		printf ("%s    [archivo de configuracion inicial]\n",
			argv[0]);
		printf ("\n-f 	Genera archivo de configuracion con patrones para funcion fxyz=sin(x) + cos(x) + z.\n");
		exit (-1);
	}

	if (strcmp (argv[1], "-f") == 0)
	{
		// Auxiliar: Ejecucion de la generacion de los patrones para aprender
		// una funcion especifica.
		generateTrainningSet ();
		exit (0);
	}

	// Configuracion de la red
	config (argv[1]);

	// Inicializacion de la red en base a la configucaion (variables globales)
	init (&W, &E);

	getValue (logFilename, "log.filename", argv[1]);

	if ((pfLogFile = fopen (logFilename, "w+")) == NULL)
	{
		printf ("Modo: no logging.\n");
	}

    getValue(logFilename, "log.input", argv[1]);
    if ((pfInput = fopen (logFilename, "w+")) == NULL)
    {
        printf ("Modo: no logging.\n");
    }
    getValue(logFilename, "log.output", argv[1]);
    if ((pfOutput = fopen (logFilename, "w+")) == NULL)
    {
        printf ("Modo: no logging.\n");
    }

	// Obtiene pesos sinapticos al azar para la matriz de pesos sinapticos.
	getRandomWeight (W);
    showNLWeight (W, Di, D);

	// Nombre del archivo de configuracion con los patrones a aprender
	getValue (patternFilename, "pattern.filename", argv[1]);

	// Inicializa los vectores X-Y donde se almacenan los patrones y los resultados deseados
	initLearningPatterns (&X, &Y, patternFilename);
	getLearningPatterns (X, Y, patternFilename);

	// Parametros sobre los patrones a aprender
	getValue (buffer, "pattern.size", patternFilename);
	patternSize = atoi (buffer);

	getValue (buffer, "showOutputFx", argv[1]);
	bShowOutputFx = atoi (buffer);

	// Realiza el ajuste de los pesos sinapticos en base a los patrones a aprender
	// y a sus salidas deseadas
	learnAll (W, E, X, Y, patternSize);

	printf ("Matriz de pesos sinapticos:\n");
	showNLWeight (W, Di, D);

	// Verifica la red con los patrones aprendidos
	for (s = 0; s < patternSize; s++)
	{
		// Entrada del bias fija
		E[0][0] = -1;
		for (i = 1; i < Di[0]; i++)
		{
			E[0][i] = X[s][i - 1];
		}
		showRNeuron (E[0], Di[0]);

		// Evoluciona la red
		evolveLayeredNN (W, E);
		if (bShowOutputFx)
			printf ("%12.10f", fx (E[0][1], E[0][2], E[0][3]));
		printf (">");
		//printf ("%12.10f",E[D][0]);
		showRNeuron (E[D], Di[D]);
		printf ("\n");

        logFile(pfInput, E[0], Di[0]);

        logFile(pfOutput, E[D], Di[D]);

	}
	
	// Cierre del archivo de log
	fclose (pfLogFile);

	exit (0);
	
	// Verifica la red con patrones generados al azar.
	for (s = 0; s < patternSize; s++)
	{
		E[0][0] = -1;	// Fixed
		for (i = 0 + 1; i < Di[0] + 1; i++)
		{
			//E[0][i]=( getNaturalMinMaxProb(-1,1)>0 ? 1 : -1);
			E[0][i] = getNaturalMinMaxProb (-1, 1);
		}

		showRNeuron (E[0], Di[0]);
		evolveLayeredNN (W, E);
		showRNeuron (E[D], Di[D]);
		printf ("\n");
	}
}
