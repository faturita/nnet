/**
 * Pure C implementation of a Multi Layer Perceptron (MLP) neural network
 * 
 * Featuring:
 * - Configurable number of layers and neurons per layer.
 * - Hyperbolic tangent activation function.
 * - Backpropagation learning algorithm.
 * - Momentum support.
 * - RMS error calculation and logging.
 * - Random weight initialization within calculated limits.
 * 
 * 
 * This will have:
 * - Adjustable precision in terms of the aproximation.
 * - Support for different activation functions.
 * - Different optimization algorithms
 * - GPU-parallelized version with OpenCL and CUDA.
 * - Support for various loss functions.
 * - KAN neural model support.
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NEURONTYPE
#define NEURONTYPE
// Input data type
typedef float neuron;
// Weight data type
typedef float weight;
#endif


#include "mathNeuron.h"
#include "arrayView.h"
#include "parameter.h"
#include "logger.h"
#include "signal.h"
#include "mlp.h"

extern char logBuffer[256];

// How many iterations with no weight changes
long REPLY_FACTOR;

// Adjustment to determine if a synaptic weight varies or not.
float ACCURACY;

// Factor on the impact of output values on the synaptic weights
// of neurons in the internal layers
float LI_E;

// Factor on the variation of synaptic weights.
float DELTA_WEIGHT;

// Momentum parameter
float MOMENTUM;

// RMS cutoff value.
float RMS_BREAK;

// How many layers for the perceptron (maximum path of the graph)
int D;

// Dimension array with the number of neurons in each layer. Dim(Di) = D+1 (because of the input)
int *Di;

unsigned int timeseed;

int forceBreak = 0;

void generateTrainingSet ()
{
    neuron x[5];
    neuron y[5];
    neuron z[5];
    neuron fx;
    int i, j, k;

    for (i = 0; i < 5; i++)
    {
        x[i] = getNaturalMinMaxProb (0, 2 * M_PI);
        y[i] = getNaturalMinMaxProb (0, 2 * M_PI);
        z[i] = getNaturalMinMaxProb (-1, 1);
    }

    printf ("pattern.size=125\n\n");
    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 5; j++)
        {
            for (k = 0; k < 5; k++)
            {
                fx = sin (x[i]) + cos (y[j]) + z[k];
                printf ("pattern.in.%d.0=%12.10f\n",
                        i * 25 + j * 5 + k,
                        (x[i] - M_PI) / (M_PI));
                printf ("pattern.in.%d.1=%12.10f\n",
                        i * 25 + j * 5 + k,
                        (y[j] - M_PI) / (M_PI));
                printf ("pattern.in.%d.2=%12.10f\n",
                        i * 25 + j * 5 + k, z[k]);
                printf ("pattern.out.%d.0=%12.10f\n\n",
                        i * 25 + j * 5 + k, fx / 3);
            }
        }
    }
}


void
initLearningPatterns (neuron *** X, neuron *** Y, char *filename)
{
    int iSize;
    int p;
    char buffer[MAXSIZE];

    printf ("Reading file...\n");

    // Dataset size parameter.
    getValue (buffer, "pattern.size", filename);
    iSize = atoi (buffer);

    *X = (neuron **) malloc (sizeof (neuron *) * iSize);
    *Y = (neuron **) malloc (sizeof (neuron *) * iSize);

    if (((*X) == NULL) || ((*Y) == NULL))
    {
        printf ("Not enough memory\n");
        exit (-1);
    }

    for (p = 0; p < iSize; p++)
    {
        // D is the number of layers (excluding input layer)
        // Di[0] is the number of input neurons
        // Di[D] is the number of output neurons (Di has size D+1 because it includes the input layer)
        *(*X + p) = (neuron *) malloc (sizeof (neuron) * Di[0]);
        *(*Y + p) = (neuron *) malloc (sizeof (neuron) * Di[D]);

        if (((*X + p) == NULL) || (*(*Y + p) == NULL))
        {
            printf ("Not enough memory.\n");
            exit (-1);
        }
    }
}

void getLearningPatterns (neuron ** X, neuron ** Y, char *filename)
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
            getValue (buffer, aux1, filename);
            X[p][j] = (neuron) atof (buffer);
        }

        for (j = 0; j < Di[D]; j++)
        {
            sprintf (aux1, "pattern.out.%d.%d", p, j);
            getValue (buffer, aux1, filename);
            Y[p][j] = (neuron) atof (buffer);
        }

    }
}


int
config (char *filename)
{
    char buffer[MAXSIZE];
    char aux1[MAXSIZE];
    int k;

    getValue (buffer, "layer.size", filename);
    D = atoi (buffer);

    getValue (buffer, "reply.factor", filename);
    REPLY_FACTOR = atol (buffer);

    getValue (buffer, "accuracy", filename);
    ACCURACY = atof (buffer);

    getValue (buffer, "delta.li", filename);
    LI_E = atof (buffer);

    getValue (buffer, "delta.weight", filename);
    DELTA_WEIGHT = atof (buffer);

    timeseed = 0;
    getValue (buffer, "timeseed", filename);
    if (strlen(buffer)>1)
        timeseed = atoi(buffer);

    MOMENTUM = 0;
    getValue(buffer, "momentum", filename);
    if (strlen(buffer)>1)
        MOMENTUM = atof(buffer);

    RMS_BREAK = 0.0001;
    getValue(buffer,"rms.break", filename);
    if (strlen(buffer)>1)
        RMS_BREAK = atof(buffer);

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
    }

    // TODO: Asserts for null pointers
    // TODO: Check if there is a misconfiguration file.
    return 1;
}



neuron fx (neuron x, neuron y, neuron z)
{
    return ((sin (x * 3.141516 + 3.141516) + cos (y * 3.141516 + 3.141516) + z) / 3);
}

void summary()
{
    int freeparameters = 0;
    printf("Number of Weight Layers %d\n", D);
    for (int k=0;k<D;k++) {
        printf ("Layer: %d M (%d neuron, of %d dim)\n",k,Di[k+1],Di[k]+1);
        freeparameters += Di[k+1]*(Di[k]+1);
    }
    printf("Total free parameters: %d\n", freeparameters);
    printf("Reply factor: %ld\n", REPLY_FACTOR);
    printf("Accuracy: %f\n", ACCURACY);
    printf("Delta Li: %f\n", LI_E);
    printf("Delta Weight: %f\n", DELTA_WEIGHT);
    printf("Momentum: %f\n", MOMENTUM);
    printf("RMS Break: %f\n", RMS_BREAK);

}

int getFanIn(int k)
{
    return Di[k];
}

int getFanOut(int k)
{
    return Di[k+1];
}

void showWeights(weight **W, int *Di, int D) {
	int i,k,j;
   
   for (k=0;k<D;k++) {
   	printf ("Layer: %d M (%d,%d)\n",k,Di[k+1],Di[k]+1);
      showWeight(W[k],Di[k+1],Di[k]+1);
   }
}

void getRandomWeights (weight ** W)
{
    int i, j, k;

    for (k = 0; k < D; k++)
    {
        float fanin, fanout;

        fanin = (float)getFanIn(k);
        fanout = (float)getFanOut(k);

        float limit = sqrtf(6.0f / (fanin + fanout));


        for (i=0;i<Di[k + 1]*(Di[k]+1);i++)
        {
            *(W[k] + i) = getNaturalMinMaxProb (-limit, limit);
        }
    }

    return;
}


float activation(float fVal) {
	return ( (exp( fVal ) - exp ( -fVal ))/(exp(fVal) + exp(-fVal)));
}

float derivative(float fVal) {
    return (1 - fVal * fVal);
}

void multiply(neuron *output, neuron * input, weight * W, int rows, int cols)
{
    //printf("Multiply %d x %d\n", rows, cols);
    float aux = 0;
    for(int i=0; i<rows; i++)
    {
        aux = 0;
        for(int j=0; j<cols; j++)
        {
            aux += input[j] * (*(W + cols * i + j));
        }
        output[i] = activation(aux);
    }
}


void transpose_multiply_skip_bias(neuron *out, neuron *in, weight *W, int rows_prev, int rows_next)
{
    // rows_prev = Di[k], rows_next = Di[k+1]
    int cols = rows_prev + 1; // because W stored with +1 bias column
    for (int i = 0; i < rows_prev; ++i) {
        float aux = 0.0f;
        for (int j = 0; j < rows_next; ++j) {
            // W[j * cols + i] is weight from neuron i in layer k to neuron j in layer k+1
            aux += in[j] * W[j * cols + i];
        }
        out[i] = aux;
    }
}



void forward(weight ** W, neuron ** E)
{
    for(int k=1; k<=D; k++)
    {
        multiply(E[k],E[k-1], W[k-1], Di[k], (Di[k-1]+1));
        E[k][Di[k]] = -1; // Fixed bias
    }
    
}


void allocate (weight *** W, weight *** dW, neuron *** E, neuron *** Li)
{
    int i = 0;

    // D represents how many layers with neurons.
    *W = (weight **) malloc (sizeof (weight *) * D);
    *dW = (weight **) malloc (sizeof (weight *) * D);
    *E = (neuron **) malloc (sizeof (neuron *) * (D + 1));
    *Li = (neuron **) malloc (sizeof (neuron *) * (D));

    if (*W == NULL || *E == NULL || *dW == NULL || *Li == NULL)
    {
        printf ("Not enough memory.\n");
        exit (-1);
    }

    // Di[i] layers size
    for (i = 0; i < (D + 1); i++)
    {
        (*E)[i] = (neuron *) malloc (sizeof (neuron) * (Di[i]+1)); // +1 for the bias
        if ((*E)[i] == NULL)
        {
            printf ("Not enough memory.\n");
            exit (-1);
        }
        memset((*E)[i], 0, sizeof (neuron) * (Di[i]+1));
    }

    // Init weight matrix
    for (i = 0; i < D; i++)
    {
        (*W)[i] =
                (weight *) malloc (sizeof (weight) *
                                   (Di[i + 1])* (Di[i]+1));

        (*dW)[i] =
                (weight *) malloc (sizeof (weight) *
                                   (Di[i + 1]) * (Di[i]+1));
        if ((*W)[i] == NULL || (*dW)[i] == NULL)
        {
            printf ("Not enough memory.\n");
            exit (-1);
        }
    }

    for (int k = D; k > 0; k--)
    {
        *(Li + (k - 1)) = (neuron *) malloc (sizeof (neuron) * Di[k]);

        memset(*(Li + (k - 1)),0,sizeof (neuron) * Di[k]);
    }
}


void cleanLi(neuron **Li)
{
    for (int k = D; k > 0; k--)
    {
        free (*(Li + (k - 1)));
    }

    free (Li);
}


void back(neuron **Li, weight ** W, neuron ** E, neuron * Y)
{
    int k,j,i;

    for (k = D; k > 0; k--)
    {
        memset(*(Li + (k - 1)),0,sizeof (neuron) * Di[k]);
    }

    for(k=D;k>0;k--)
    {
        if (k==D)
        {
            for(int j=0;j<Di[k];j++)
                Li[k-1][j] = derivative(E[k][j]) * (E[k][j] - Y[j]);
        }
        else
        {
            //amultiply(Li[k-1], Li[k], W[k], Di[k], Di[k+1]);
            transpose_multiply_skip_bias(Li[k-1], Li[k], W[k], Di[k], Di[k+1]);
            for(int j=0;j<Di[k];j++)
                Li[k-1][j] = derivative(E[k][j]) * Li[k-1][j];
        }

                        //dW = (weight) ((DELTA_WEIGHT) *
                        //       Li[(k) - 1][i] * E[k - 1][j]);

        // Update weights
        //dW = 0.001 * L * E[k-1];       

    }

    // return Li

}


void sigintHandler(int sig_num)
{
    signal(SIGINT, sigintHandler);
    forceBreak = 1;
    fflush(stdout);
}


FILE *pfLogFile;
FILE *pfInput;
FILE *pfOutput;



void loadPattern(neuron * dest, neuron * X)
{
	for (int i = 0; i < Di[0]; i++)
	{
		dest[i] = X[i];
	}
	dest[Di[0]] = -1; // Fixed bias
}

float getQuadraticError (float **W, float **E, float **X, float **Y,
                   int patternSize)
{
    int p, j;
    float fQErr=0;

    for (p = 0; p < patternSize; p++)
    {
		loadPattern(E[0],X[p]);
		forward(W, E);	
        for (j=0;j<Di[D];j++)
        {
            fQErr += (pow((Y[p][j] - E[D][j]),2));
        }

    }

    fQErr = fQErr / (float) patternSize;
    return fQErr;
}

float logQuadraticError (weight ** W, neuron ** E, neuron ** X, neuron ** Y,
                   int patternSize)
{
    float rms=getQuadraticError (W, E, X, Y, patternSize);
    if (TRUE)
    {
        sprintf (logBuffer, "%12.10f\n",
                 rms);
        logInfo (logBuffer);
    }
    return rms;
}

int checkBinary(neuron *Ei,int iSizeInput, neuron *Eo, int iSizeOutput)
{
    int oks = 0;

    assert(iSizeInput == iSizeOutput || !"Size mismatch in checkBinary");

    for(int i=0;i<iSizeInput;i++)
    {
        if (sgn(Ei[i]) == sgn(Eo[i]))
            oks++;
    }
    return oks;
}


/**
 * E are the neuron outputs, E[0] input layer, E[D] output layer.  E[k] is of size Di[k]+1 (bias). E[k][Di[k]] = -1
 * X are the input patterns, of size patternSize x Di[0]
 * Y are the target outputs, of size patternSize x Di[D]
 * 
 * W are the synaptic weights, W[k] is the weight matrix between layer k and k+1, of size Di[k+1] x (Di[k]+1)
 *      So W[k] has Di[k+1] neurons rows and Di[k]+1 columns inputs to the layer (including the extra bias).  
 * 
 * dW are the delta weights, same size as W.
 * 
 * Li are the local gradients, Li[k] is of size Di[k].
 * 
 * 
 * 
 */
int main (int argc, char *argv[])
{
	weight **W;		// weights
	neuron **E;		// input layer, and neuron outputs.
	neuron **Y;		// labels
	neuron **X;		// training set

	weight **dW;	// Delta sinaptic weights.
    neuron **Li;	// Local gradients

	char buffer[MAXSIZE];	// Buffer auxiliar
	char patternFilename[MAXSIZE];	
	char logFilename[MAXSIZE];	
	int patternSize;	
	int i, s;		
	int bShowOutputFx = 0;	

    initRandom (0);

	printf ("Pure C implementation of a Multi Layer Perceptron (MLP) neural network\n");
    signal(SIGINT, sigintHandler);

	if (argc != 2)
	{

		exit (-1);
	}

	if (strcmp (argv[1], "-f") == 0)
	{
		generateTrainingSet ();
		exit (0);
	}

	// Get network architecture
	config (argv[1]);

	getValue (logFilename, "log.filename", argv[1]);

	if ((pfLogFile = fopen (logFilename, "w+")) == NULL)
	{
		printf ("No log.\n");
	}

    getValue(logFilename, "log.input", argv[1]);
    if ((pfInput = fopen (logFilename, "w+")) == NULL)
    {
        printf ("No log.\n");
    }
    getValue(logFilename, "log.output", argv[1]);
    if ((pfOutput = fopen (logFilename, "w+")) == NULL)
    {
        printf ("No log.\n");
    }

	allocate (&W, &dW, &E, &Li);

	getRandomWeights (W);
    showWeights (W, Di, D);

	getValue (patternFilename, "pattern.filename", argv[1]);

	initLearningPatterns (&X, &Y, patternFilename);
	getLearningPatterns (X, Y, patternFilename);

	getValue (buffer, "pattern.size", patternFilename);
	patternSize = atoi (buffer);

	getValue (buffer, "showOutputFx", argv[1]);
	bShowOutputFx = atoi (buffer);

	summary();

	int iChance = getProb (0, patternSize);
	int patternIndex = iChance;
	loadPattern(E[0],X[iChance]);
	showRNeuron (E[0], Di[0]+1);printf ("\n");
	forward(W, E);
	showRNeuron (E[D], Di[D]);printf ("\n");


	neuron **Li = initLi();

	float eta = DELTA_WEIGHT;

    long tries = 0;

    float rms = 0.0f;

    int updates = 0;

	while (tries++ < REPLY_FACTOR && !forceBreak)
	{
		iChance = getProb (0, patternSize);
		loadPattern(E[0],X[iChance]);
		forward(W, E);						// Updates E
		back(Li,W,E,Y[iChance]);			// Updates Li

		for (int k = 0; k < D; k++)
    	{
        	for (int i = 0; i < Di[k+1]; i++)
        	{
				int cols = Di[k]+1;
            	for (int j = 0; j < Di[k]+1; j++)
            	{
					//printf("dW[%d][%d][%d]\n",k,i,j);
                	 *(*(dW + k) + i * cols + j)   = (weight) ((- eta) *
                               Li[k][i] * E[k][j]);
				}
			}
		}

		for (int k = 0; k < D; k++)
    	{
        	for (int i = 0; i < Di[k+1]; i++)
        	{
				int cols = Di[k]+1;
            	for (int j = 0; j < Di[k]+1; j++)
            	{
					//printf("dW[%d][%d][%d]\n",k,i,j);
                	 *(*(W + k) + i * cols + j)   += *(*(dW + k) + i * cols + j); 
				}
			}
		}

        if ((tries % patternSize) == 0)
        {
            rms = logQuadraticError (W, E, X, Y, patternSize);
            if (rms < RMS_BREAK)
                break;

        }

        if (tries % 100000 == 0)
        {
            printf("%ld:%d\n", tries, updates);
        }

	}

    cleanLi(Li);

	// loadPattern(E[0],X[patternIndex]);
	// showRNeuron (E[0], Di[0]+1);printf ("\n");
	// forward(W, E);
	// printf ("%12.10f", fx (E[0][1], E[0][2], E[0][3]));printf (">");
	// showRNeuron (E[D], Di[D]);printf ("\n");


    int acc = 0;
    int oks = 0;
	for(int s=0;s<patternSize;s++)
	{
		iChance = s;
		loadPattern(E[0],X[iChance]);
		showRNeuron (E[0], Di[0]+1);        // +1 for bias
		forward(W, E);
		if (bShowOutputFx)
			printf (" %12.10f", fx (E[0][0], E[0][1], E[0][2]));
		
		printf (">");	
		showRNeuron (E[D], Di[D]);printf ("\n");
        int res = checkBinary(E[0],Di[0], E[D], Di[D]);
        oks += res;
        if (res == Di[0])
            acc++;
	}

    printf("Success %d/%d rate: %10.4f\n", acc,patternSize, (acc/(float)patternSize));
    printf("OKs: %d, %d\n", oks, patternSize * Di[0]);
    printf("Seed: %u\n", timeseed);
    printf("Tries: %ld\n", tries);
    printf("Final RMS: %12.10f\n", rms);

    return 0;
}

