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

#include "signal.h"


// Declare and Init the font array data
int const Font1[32][7] = {
   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // 0x20, space
   {0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04},   // 0x21, !
   {0x09, 0x09, 0x12, 0x00, 0x00, 0x00, 0x00},   // 0x22, "
   {0x0a, 0x0a, 0x1f, 0x0a, 0x1f, 0x0a, 0x0a},   // 0x23, #
   {0x04, 0x0f, 0x14, 0x0e, 0x05, 0x1e, 0x04},   // 0x24, $
   {0x19, 0x19, 0x02, 0x04, 0x08, 0x13, 0x13},   // 0x25, %
   {0x04, 0x0a, 0x0a, 0x0a, 0x15, 0x12, 0x0d},   // 0x26, &
   {0x04, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00},   // 0x27, '
   {0x02, 0x04, 0x08, 0x08, 0x08, 0x04, 0x02},   // 0x28, (
   {0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08},   // 0x29, )
   {0x04, 0x15, 0x0e, 0x1f, 0x0e, 0x15, 0x04},   // 0x2a, *
   {0x00, 0x04, 0x04, 0x1f, 0x04, 0x04, 0x00},   // 0x2b, +
   {0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x08},   // 0x2c, ,
   {0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00},   // 0x2d, -
   {0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c},   // 0x2e, .
   {0x01, 0x01, 0x02, 0x04, 0x08, 0x10, 0x10},   // 0x2f, /
   {0x0e, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0e},   // 0x30, 0
   {0x04, 0x0c, 0x04, 0x04, 0x04, 0x04, 0x0e},   // 0x31, 1
   {0x0e, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1f},   // 0x32, 2
   {0x0e, 0x11, 0x01, 0x06, 0x01, 0x11, 0x0e},   // 0x33, 3
   {0x02, 0x06, 0x0a, 0x12, 0x1f, 0x02, 0x02},   // 0x34, 4
   {0x1f, 0x10, 0x1e, 0x01, 0x01, 0x11, 0x0e},   // 0x35, 5
   {0x06, 0x08, 0x10, 0x1e, 0x11, 0x11, 0x0e},   // 0x36, 6
   {0x1f, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},   // 0x37, 7
   {0x0e, 0x11, 0x11, 0x0e, 0x11, 0x11, 0x0e},   // 0x38, 8
   {0x0e, 0x11, 0x11, 0x0f, 0x01, 0x02, 0x0c},   // 0x39, 9
   {0x00, 0x0c, 0x0c, 0x00, 0x0c, 0x0c, 0x00},   // 0x3a, :
   {0x00, 0x0c, 0x0c, 0x00, 0x0c, 0x04, 0x08},   // 0x3b, ;
   {0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02},   // 0x3c, <
   {0x00, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x00},   // 0x3d, =
   {0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08},   // 0x3e, >
   {0x0e, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04}   // 0x3f, ?
   };

int const Font2[32][7] = {
   {0x0e, 0x11, 0x17, 0x15, 0x17, 0x10, 0x0f},   // 0x40, @
   {0x04, 0x0a, 0x11, 0x11, 0x1f, 0x11, 0x11},   // 0x41, A
   {0x1e, 0x11, 0x11, 0x1e, 0x11, 0x11, 0x1e},   // 0x42, B
   {0x0e, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0e},   // 0x43, C
   {0x1e, 0x09, 0x09, 0x09, 0x09, 0x09, 0x1e},   // 0x44, D
   {0x1f, 0x10, 0x10, 0x1c, 0x10, 0x10, 0x1f},   // 0x45, E
   {0x1f, 0x10, 0x10, 0x1f, 0x10, 0x10, 0x10},   // 0x46, F
   {0x0e, 0x11, 0x10, 0x10, 0x13, 0x11, 0x0f},   // 0x37, G
   {0x11, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11},   // 0x48, H
   {0x0e, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0e},   // 0x49, I
   {0x1f, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0c},   // 0x4a, J
   {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11},   // 0x4b, K
   {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f},   // 0x4c, L
   {0x11, 0x1b, 0x15, 0x11, 0x11, 0x11, 0x11},   // 0x4d, M
   {0x11, 0x11, 0x19, 0x15, 0x13, 0x11, 0x11},   // 0x4e, N
   {0x0e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e},   // 0x4f, O
   {0x1e, 0x11, 0x11, 0x1e, 0x10, 0x10, 0x10},   // 0x50, P
   {0x0e, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0d},   // 0x51, Q
   {0x1e, 0x11, 0x11, 0x1e, 0x14, 0x12, 0x11},   // 0x52, R
   {0x0e, 0x11, 0x10, 0x0e, 0x01, 0x11, 0x0e},   // 0x53, S
   {0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},   // 0x54, T
   {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e},   // 0x55, U
   {0x11, 0x11, 0x11, 0x11, 0x11, 0x0a, 0x04},   // 0x56, V
   {0x11, 0x11, 0x11, 0x15, 0x15, 0x1b, 0x11},   // 0x57, W
   {0x11, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x11},   // 0x58, X
   {0x11, 0x11, 0x0a, 0x04, 0x04, 0x04, 0x04},   // 0x59, Y
   {0x1f, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1f},   // 0x5a, Z
   {0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0e},   // 0x5b, [
   {0x10, 0x10, 0x08, 0x04, 0x02, 0x01, 0x01},   // 0x5c, \\
   {0x0e, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0e},   // 0x5d, ]
   {0x04, 0x0a, 0x11, 0x00, 0x00, 0x00, 0x00},   // 0x5e, ^
   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f}   // 0x5f, _
   };

int const Font3[32][7] = {
   {0x04, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00},   // 0x60, `
   {0x00, 0x0e, 0x01, 0x0d, 0x13, 0x13, 0x0d},   // 0x61, a
   {0x10, 0x10, 0x10, 0x1c, 0x12, 0x12, 0x1c},   // 0x62, b
   {0x00, 0x00, 0x00, 0x0e, 0x10, 0x10, 0x0e},   // 0x63, c
   {0x01, 0x01, 0x01, 0x07, 0x09, 0x09, 0x07},   // 0x64, d
   {0x00, 0x00, 0x0e, 0x11, 0x1f, 0x10, 0x0f},   // 0x65, e
   {0x06, 0x09, 0x08, 0x1c, 0x08, 0x08, 0x08},   // 0x66, f
   {0x0e, 0x11, 0x13, 0x0d, 0x01, 0x01, 0x0e},   // 0x67, g
   {0x10, 0x10, 0x10, 0x16, 0x19, 0x11, 0x11},   // 0x68, h
   {0x00, 0x04, 0x00, 0x0c, 0x04, 0x04, 0x0e},   // 0x69, i
   {0x02, 0x00, 0x06, 0x02, 0x02, 0x12, 0x0c},   // 0x6a, j
   {0x10, 0x10, 0x12, 0x14, 0x18, 0x14, 0x12},   // 0x6b, k
   {0x0c, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},   // 0x6c, l
   {0x00, 0x00, 0x0a, 0x15, 0x15, 0x11, 0x11},   // 0x6d, m
   {0x00, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11},   // 0x6e, n
   {0x00, 0x00, 0x0e, 0x11, 0x11, 0x11, 0x0e},   // 0x6f, o
   {0x00, 0x1c, 0x12, 0x12, 0x1c, 0x10, 0x10},   // 0x70, p
   {0x00, 0x07, 0x09, 0x09, 0x07, 0x01, 0x01},   // 0x71, q
   {0x00, 0x00, 0x16, 0x19, 0x10, 0x10, 0x10},   // 0x72, r
   {0x00, 0x00, 0x0f, 0x10, 0x0e, 0x01, 0x1e},   // 0x73, s
   {0x08, 0x08, 0x1c, 0x08, 0x08, 0x09, 0x06},   // 0x74, t
   {0x00, 0x00, 0x11, 0x11, 0x11, 0x13, 0x0d},   // 0x75, u
   {0x00, 0x00, 0x11, 0x11, 0x11, 0x0a, 0x04},   // 0x76, v
   {0x00, 0x00, 0x11, 0x11, 0x15, 0x15, 0x0a},   // 0x77, w
   {0x00, 0x00, 0x11, 0x0a, 0x04, 0x0a, 0x11},   // 0x78, x
   {0x00, 0x11, 0x11, 0x0f, 0x01, 0x11, 0x0e},   // 0x79, y
   {0x00, 0x00, 0x1f, 0x02, 0x04, 0x08, 0x1f},   // 0x7a, z
   {0x06, 0x08, 0x08, 0x10, 0x08, 0x08, 0x06},   // 0x7b, {
   {0x04, 0x04, 0x04, 0x00, 0x04, 0x04, 0x04},   // 0x7c, |
   {0x0c, 0x02, 0x02, 0x01, 0x02, 0x02, 0x0c},   // 0x7d, }
   {0x08, 0x15, 0x02, 0x00, 0x00, 0x00, 0x00},   // 0x7e, ~
   {0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f}   // 0x7f, DEL
   };


void generateFonts()
{
    printf ("pattern.size=32\n\n");

    int x[7*5];

    memset(x,0,sizeof(int)*(7*5));

    for(int p=0;p<32;p++)
    {
        for(int i=0,h=0;i<7;i++)
        {
            int val = Font1[p][i];

            for(int r=4;r>=0;r--)
            {
                int binaryval = (val >> r) & 0x01;
                if (binaryval == 0) binaryval = -1;
                x[h*5 + 4-r] = binaryval;
            }

            h++;
        }

        for(int i=0;i<7*5;i++)
        {
            printf ("pattern.in.%d.%d=%d\n",p,i,x[i]);
        }

        for(int i=0;i<7*5;i++)
        {
            printf ("pattern.out.%d.%d=%d\n",p,i,x[i]);
        }
    }

}

int checkBinary(neuron *Ei,int iSizeInput, neuron *Eo, int iSizeOutput)
{
    int ret = 1;

    if (iSizeInput != (iSizeOutput+1)) exit(-1);

    for(int i=1;i<iSizeInput;i++)
    {
        if (sgn(Ei[i]) != sgn(Eo[i-1]))
            return (ret=0);
    }
    return ret;
}
/**
 * Genera por stdout la salida con los valores
 * de configuracion con los patrones a aprender
 *
 **/
    void
generateAutoencoderTrainningSet (int iSize)
{
    int x[iSize];
    int i, j, k;

    printf ("pattern.size=125\n\n");
    for (i = 0; i < 125; i++)
    {
        for (j = 0; j < iSize; j++)
        {
            x[j] = ( getNaturalMinMaxProb(-1,1)>0 ? 1 : -1);
        }
        for (j = 0; j < iSize; j++)
        {
            printf ("pattern.in.%d.%d=%d\n",
                i,j,x[j]);
        }
        for (j = 0; j < iSize; j++)
        {
            printf ("pattern.out.%d.%d=%d\n",
                i,j,x[j]);
        }

    }
}


void sigintHandler(int sig_num)
{
    /* Reset handler to catch SIGINT next time.
       Refer http://en.cppreference.com/w/c/program/signal */
    signal(SIGINT, sigintHandler);
    forceBreak = 1;
    fflush(stdout);
}


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

    printf ("Autoencoders\n");
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
    summary();

    // Inicializacion de la generacion de numeros pseudoaleatorios
    timeseed = initRandom (timeseed);

    if ((argc >2 && strcmp (argv[2], "-f") == 0))
	{
		// Auxiliar: Ejecucion de la generacion de los patrones para aprender
		// una funcion especifica.
        //generateAutoencoderTrainningSet (Di[0]);
        generateFonts();
		exit (0);
	}

	// Inicializacion de la red en base a la configucaion (variables globales)
	init (&W, &E);

    printf("Network initialized\n");

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

	// Nombre del archivo de configuracion con los patrones a aprender
	getValue (patternFilename, "pattern.filename", argv[1]);

    printf("Pattern filename %s\n", patternFilename);

	// Inicializa los vectores X-Y donde se almacenan los patrones y los resultados deseados
	initLearningPatterns (&X, &Y, patternFilename);
	getLearningPatterns (X, Y, patternFilename);

    printf("Ready to learn\n");

	// Parametros sobre los patrones a aprender
	getValue (buffer, "pattern.size", patternFilename);
	patternSize = atoi (buffer);

	getValue (buffer, "showOutputFx", argv[1]);
	bShowOutputFx = atoi (buffer);

	// Realiza el ajuste de los pesos sinapticos en base a los patrones a aprender
	// y a sus salidas deseadas
    //learnAll (W, E, X, Y, patternSize);

    //batchLearn(W,E,X,Y,patternSize);

    batchPowelLearn(W, E, X, Y, patternSize);

	printf ("Matriz de pesos sinapticos:\n");
	showNLWeight (W, Di, D);

    saveWeight("autoencoder.weights", W);

    int acc = 0;
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
		printf (">");
		//printf ("%12.10f",E[D][0]);
        showRNeuron (E[D], Di[D]);
		printf ("\n");

        logFile(pfInput, E[0], Di[0]);

        logFile(pfOutput, E[D], Di[D]);

        if (checkBinary(E[0],Di[0], E[D], Di[D]))
        {
            printf ("===> Ok\n");acc++;
        }
        else
        {
            printf ("===> Err \n");
        }


	}

    printf("Success %d/%d rate: %10.4f\n", acc,patternSize, (acc/(float)patternSize));

    printf("Seed: %u\n", timeseed);
	
	// Cierre del archivo de log
	fclose (pfLogFile);

    exit (0); // END
	
    // Verifica la red con patrones generados al azar.
    for (s = 0; s < patternSize; s++)
    {
        E[0][0] = -1;	// Fixed
        for (i = 0 + 1; i < Di[0] + 1; i++)
        {
            E[0][i]=( getNaturalMinMaxProb(-1,1)>0 ? 1 : -1);
            //E[0][i] = getNaturalMinMaxProb (-1, 1);
        }

        showRNeuron (E[0], Di[0]);
        evolveLayeredNN (W, E);
        showRNeuron (E[D], Di[D]);
        printf ("\n");
    }
}
