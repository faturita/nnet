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


// Define Adam hyperparameters
const double beta1 = 0.9;
const double beta2 = 0.999;
const double epsilon = 1e-8;

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

int noOfFreeParameters()
{
    int freeparameters = 0;

    for (int k=0;k<D;k++) {
        freeparameters += Di[k+1]*(Di[k]+1);
    }
        
    return freeparameters;
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


void allocate (weight *** W, weight *** dW, neuron *** E, neuron *** Li, weight *** M, weight *** V)
{
    int i = 0;

    // D represents how many layers with neurons.
    *W = (weight **) malloc (sizeof (weight *) * D);

    *M = (weight **) malloc (sizeof (weight *) * D);
    *V = (weight **) malloc (sizeof (weight *) * D);


    *dW = (weight **) malloc (sizeof (weight *) * D);
    *E = (neuron **) malloc (sizeof (neuron *) * (D + 1));
    *Li = (neuron **) malloc (sizeof (neuron *) * (D));

    if (*W == NULL || *E == NULL || *dW == NULL || *Li == NULL || *M == NULL || *V == NULL)
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

        (*M)[i] =
                (weight *) malloc (sizeof (weight) *
                                   (Di[i + 1]) * (Di[i]+1));

        (*V)[i] =
                (weight *) malloc (sizeof (weight) *
                                   (Di[i + 1]) * (Di[i]+1));



        if ((*W)[i] == NULL || (*dW)[i] == NULL || (*M)[i] == NULL || (*V)[i] == NULL)
        {
            printf ("Not enough memory.\n");
            exit (-1);
        }
    }


    for(i = 0; i < D; i++)
    {
        memset((*W)[i], 0, sizeof (weight) * (Di[i + 1]) * (Di[i]+1));
        memset((*dW)[i], 0, sizeof (weight) * (Di[i + 1]) * (Di[i]+1));
        memset((*M)[i], 0, sizeof (weight) * (Di[i + 1]) * (Di[i]+1));
        memset((*V)[i], 0, sizeof (weight) * (Di[i + 1]) * (Di[i]+1));
    }




    for (int k = D; k > 0; k--)
    {
        (*Li)[k - 1] = (neuron *) malloc (sizeof (neuron) * Di[k]);

        memset((*Li)[k - 1],0,sizeof (neuron) * Di[k]);
    }
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


void serializeWeights(weight ** W, double *v)
{
    int k, i;
    int index=0;


    for (k = 0; k < D; k++)
    {
        for (i=0;i<Di[k + 1]*(Di[k]+1);i++)
        {
                v[index++] = *(W[k] + i) ;
        }
    }
}

void unserializeWeights(weight ** W, double *v)
{
    int k, i;
    int index=0;

    for (k = 0; k < D; k++)
    {
        for (i=0;i<Di[k + 1]*(Di[k]+1);i++)
        {
            *(W[k] + i) =v[index++];
        }
    }
}

void saveWeight(char sFileName[], weight **W)
{
    FILE *pf = fopen(sFileName,"wb");

    double *v;
    int size;

    size = noOfFreeParameters();

    v = (double*) malloc(sizeof(double) * size);

    serializeWeights(W,v);

    fwrite(v,sizeof(double),size,pf);

    free(v);

    fclose(pf);
}

/**
 * Carga de sFileName una copia con los valores de los pesos sinapticos W
 *
 *
 **/
void loadWeight(char sFileName[], weight **W)
{
    FILE *pf = fopen(sFileName,"rb");

    double *v;
    int size;

    size = noOfFreeParameters();

    v = (double*) malloc(sizeof(double) * size);

    fread(v,sizeof(double),size,pf);

    // W pointer here is already initialized.  This function just fills-in the values.
    unserializeWeights(W,v);

    free(v);

    fclose(pf);
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

void batchUpdate(weight **W, float eta, weight **dW)
{
    for (int k = 0; k < D; k++)
    {
        for (int i = 0; i < Di[k + 1]; i++)
        {
            int cols = Di[k] + 1;
            for (int j = 0; j < Di[k] + 1; j++)
            {
                // printf("dW[%d][%d][%d]\n",k,i,j);
                *(*(W + k) + i * cols + j) +=  (-eta) * (*(*(dW + k) + i * cols + j));
            }
        }
    }
}

void adamUpdate(weight **W, float eta, weight **dW, weight **M, weight **V, int t, int batchsize)
{
    for (int k = 0; k < D; k++)
    {
        for (int i = 0; i < Di[k+1]; i++)   
        {
            int cols = Di[k]+1;
            for (int j = 0; j < Di[k]+1; j++)
            {
                // 1. Get the average gradient over the mini-batch
                // (We summed gradients in dW, now we divide by batchsize)
                weight g_t = *(*(dW + k) + i * cols + j) / (weight)batchsize;

                // 2. Get old moments
                weight m_old = *(*(M + k) + i * cols + j);
                weight v_old = *(*(V + k) + i * cols + j);

                // 3. Update biased first moment (m)
                weight m_t = beta1 * m_old + (1.0 - beta1) * g_t;
                *(*(M + k) + i * cols + j) = m_t;

                // 4. Update biased second moment (v)
                weight v_t = beta2 * v_old + (1.0 - beta2) * (g_t * g_t);
                *(*(V + k) + i * cols + j) = v_t;

                // 5. Compute bias-corrected first moment (m_hat)
                weight m_hat_t = m_t / (1.0 - pow(beta1, t));

                // 6. Compute bias-corrected second moment (v_hat)
                weight v_hat_t = v_t / (1.0 - pow(beta2, t));

                // 7. Update weight (W)
                // W = W - eta * m_hat / (sqrt(v_hat) + epsilon)
                *(*(W + k) + i * cols + j) -= eta * m_hat_t / (sqrt(v_hat_t) + epsilon);
            }
        }
    }
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

    weight **M;    // First moment estimates for Adam
    weight **V;    // Second moment estimates for Adam

	char buffer[MAXSIZE];	// Buffer auxiliar
	char patternFilename[MAXSIZE];	
	char logFilename[MAXSIZE];	
	int patternSize;	
	int i, s;		
	int showOutputFx = 0;	
    int calculateAccuracyBinary = 0;

	printf ("Pure C implementation of a Multi Layer Perceptron (MLP) neural network\n");
    signal(SIGINT, sigintHandler);

	// Get network architecture
	config (argv[1]);

    timeseed = initRandom (timeseed);

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

	allocate (&W, &dW, &E, &Li, &M, &V);

    if ((argc >2 && strcmp (argv[2], "-l") == 0))
	{
        printf("Loading weights from file mlp.weights\n");
        loadWeight("mlp.weights", W);
    }
    else
    {
        getRandomWeights (W);   

    }

    showWeights (W, Di, D);

	getValue (patternFilename, "pattern.filename", argv[1]);

	initLearningPatterns (&X, &Y, patternFilename);
	getLearningPatterns (X, Y, patternFilename);

	getValue (buffer, "pattern.size", patternFilename);
	patternSize = atoi (buffer);

	getValue (buffer, "showOutputFx", argv[1]);
	showOutputFx = atoi (buffer);

    getValue (buffer, "calculateAccuracyBinary", argv[1]);
    calculateAccuracyBinary = atoi (buffer);

	summary();

	int iChance = getProb (0, patternSize);
	int patternIndex = iChance;
	loadPattern(E[0],X[iChance]);
	showRNeuron (E[0], Di[0]+1);printf ("\n");
	forward(W, E);
	showRNeuron (E[D], Di[D]);printf ("\n");

	float eta = DELTA_WEIGHT;

    long tries = 0;

    float rms = 0.0f, lastRms = 0.0f;

    int updates = 0;

    int batchsize = patternSize;

	while (tries++ < REPLY_FACTOR && !forceBreak)
	{
        int arr[patternSize];
        memset(arr,0,sizeof(int)*patternSize);

        for(int i=0;i<patternSize;i++) { arr[i]=i; }
            shuffle(arr, patternSize, sizeof(int));

        for (i = 0; i < D; i++)
        {
            int inputdimension = Di[i+1];
            int neurons = Di[i] + 1;
            int num_elements = neurons * inputdimension;
            memset(*(dW + i ), 0, sizeof(weight) * num_elements); 
        }
        
        for(int b=0; b<batchsize;b++)
        {
            //iChance = getProb (0, patternSize);
            iChance = b;
            loadPattern(E[0],X[arr[iChance]]);
            forward(W, E);						// Updates E
            back(Li,W,E,Y[arr[iChance]]);			// Updates Li

            for (int k = 0; k < D; k++)
            {
                for (int i = 0; i < Di[k+1]; i++)
                {
                    int cols = Di[k]+1;
                    for (int j = 0; j < Di[k]+1; j++)
                    {
                        //printf("dW[%d][%d][%d]\n",k,i,j);
                        weight raw_grad = Li[k][i] * E[k][j];

                        *(*(dW + k) + i * cols + j)   += raw_grad;
                    }
                }
            }
        }

        //batchUpdate(W, eta, dW);
        adamUpdate(W, eta, dW, M, V, tries, batchsize);


        if ((tries % patternSize) == 0)
        {
            rms = logQuadraticError (W, E, X, Y, patternSize);
            if (rms < RMS_BREAK)
                break;

        }


        if (tries % 100000 == 0)
        {
            printf("%ld:%d\n", tries, updates);

            printf("RMS: %12.10f LastRMS: %12.10f Eta: %f\n", rms, lastRms, eta);

        }

	}

	// loadPattern(E[0],X[patternIndex]);
	// showRNeuron (E[0], Di[0]+1);printf ("\n");
	// forward(W, E);
	// printf ("%12.10f", fx (E[0][1], E[0][2], E[0][3]));printf (">");
	// showRNeuron (E[D], Di[D]);printf ("\n");


    int acc=0,acc1 = 0;
    int oks = 0;
	for(int s=0;s<patternSize;s++)
	{
		iChance = s;
		loadPattern(E[0],X[iChance]);
		showRNeuron (E[0], Di[0]+1);        // +1 for bias
		forward(W, E);
		if (showOutputFx)
			printf (" %12.10f", fx (E[0][0], E[0][1], E[0][2]));
		
		printf (">");	
		showRNeuron (E[D], Di[D]);printf ("\n");

        if (calculateAccuracyBinary)
        {
            int res = checkBinary(E[D],Di[D], Y[iChance], Di[D]);
            oks += res;
            if (res == Di[D])
                acc++;
            if (res >= (Di[D]-1))
                acc1++;
        }
	}

    saveWeight("mlp.weights", W);

    printf("Success %d/%d rate: %10.4f\n", acc,patternSize, (acc/(float)patternSize));
    printf("OKs: %d, %d\n", oks, patternSize * Di[0]);
    printf("Seed: %u\n", timeseed);
    printf("Tries: %ld\n", tries);
    printf("Final RMS: %12.10f\n", rms);
    printf("Final Eta: %f\n", eta);
    printf("1-Bit error success %d/%d rate: %10.4f\n", acc1,patternSize, (acc1/(float)patternSize));

    return 0;
}


