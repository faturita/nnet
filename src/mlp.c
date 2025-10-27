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

#include "layeredNNet.c"


void summarize()
{
    printf("Number of Weight Layers %d\n", D);
    for (int k=0;k<D;k++) {
        printf ("Layer: %d M (%d neuron, of %d dim)\n",k,Di[k+1],Di[k]+1);
    }
}

void showWeights(float **W, int *Di, int D) {
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


        for (i=0;i<Di[k + 1]*(Di[k]+1);i++)
        {
            *(W[k] + i) = getNaturalMinMaxProb (-1, 1) * (sqrt(6.0)/sqrtf(fanin+fanout));
        }
    }

    return;
}


extern int D;
extern int *Di;

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


void
allocate (weight *** W, weight *** dW, neuron *** E)
{
    int i = 0;

    // D represents how many layers with neurons.
    *W = (weight **) malloc (sizeof (weight *) * D);
    *dW = (weight **) malloc (sizeof (weight *) * D);
    *E = (neuron **) malloc (sizeof (neuron *) * (D + 1));

    if (*W == NULL || *E == NULL || *dW == NULL)
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
}

neuron** initLi()
{
    neuron **Li = (neuron **) malloc (sizeof (neuron *) * (D));

    for (int k = D; k > 0; k--)
    {
        *(Li + (k - 1)) = (neuron *) malloc (sizeof (neuron) * Di[k]);

        memset(*(Li + (k - 1)),0,sizeof (neuron) * Di[k]);
    }

    return Li;
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
    /* Reset handler to catch SIGINT next time.
       Refer http://en.cppreference.com/w/c/program/signal */
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


int
main (int argc, char *argv[])
{
	weight **W;		
	neuron **E;		
	neuron **Y;		// labels
	neuron **X;		// training set

	weight **dW;	// Delta sinaptic weights.

	char buffer[MAXSIZE];	// Buffer auxiliar
	char patternFilename[MAXSIZE];	
	char logFilename[MAXSIZE];	
	int patternSize;	
	int i, s;		
	int bShowOutputFx = 0;	

    initRandom (0);

	printf ("Backpropagation\n");
    signal(SIGINT, sigintHandler);

	if (argc != 2)
	{

		exit (-1);
	}

	if (strcmp (argv[1], "-f") == 0)
	{
		generateTrainningSet ();
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

	allocate (&W, &dW, &E);

	getRandomWeights (W);
    showWeights (W, Di, D);

	getValue (patternFilename, "pattern.filename", argv[1]);

	initLearningPatterns (&X, &Y, patternFilename);
	getLearningPatterns (X, Y, patternFilename);

	getValue (buffer, "pattern.size", patternFilename);
	patternSize = atoi (buffer);

	getValue (buffer, "showOutputFx", argv[1]);
	bShowOutputFx = atoi (buffer);

	summarize();

	int iChance = getProb (0, patternSize);
	int patternIndex = iChance;
	loadPattern(E[0],X[iChance]);
	showRNeuron (E[0], Di[0]+1);printf ("\n");
	forward(W, E);
	showRNeuron (E[D], Di[D]);printf ("\n");


	neuron **Li = initLi();

	float eta = 0.001;

	for(int tries=0;tries<1000000000;tries++)
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

	}

	// loadPattern(E[0],X[patternIndex]);
	// showRNeuron (E[0], Di[0]+1);printf ("\n");
	// forward(W, E);
	// printf ("%12.10f", fx (E[0][1], E[0][2], E[0][3]));printf (">");
	// showRNeuron (E[D], Di[D]);printf ("\n");


	for(int s=0;s<patternSize;s++)
	{
		iChance = s;
		loadPattern(E[0],X[iChance]);
		showRNeuron (E[0], Di[0]+1);
		forward(W, E);
		if (bShowOutputFx)
			printf ("%12.10f", fx (E[0][1], E[0][2], E[0][3]));
		
		printf (">");	
		showRNeuron (E[D], Di[D]);printf ("\n");
	}

    return 0;
}

