/**
 * Implementacion perceptron simple.
 *
 * Para modelar el apendizaje de los pesos sinapticos va bien un
 * array de arrays, donde cada array representa una capa del perceptron
 * (en el caso del simple solo hay una capa pero la generalizacion va a servir)
 *
 * N dimension del vector de entrada X
 * M dimension del vector de salida O
 *
 * D cantidad de capas del perceptron (cantidad de arrays de pesos sinapticos)
 *
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NEURONTYPE
#define NEURONTYPE
// Tipo de dato para las salidas y entradas de las neuronas
typedef float neuron;
// Tipo de dato para los pesos sinapticos
typedef float weight;
#endif


#include "mathNeuron.h"
#include "arrayView.h"
#include "parameter.h"
#include "logger.h"


// Cantidad de iteraciones con variaciones de los pesos sinapticos nulas
long REPLY_FACTOR;

// Ajuste sobre el cual se determina si un peso sinaptico varia o no.
float ACCURACY;

// Factor sobre el impacto de los valores de salida sobre los pesos sinapticos
// de las neuronas en las capas internas
float LI_E;

// Factor de variacion de los pesos sinapticos.
float DELTA_WEIGHT;

// Parametro del momentum
float MOMENTUM;

// Valor de corte de RMS.
float RMS_BREAK;

// Cantidad de layers para el perceptron (camino maximo del grafo)
int D;

// Array con la cantidad de neuronas en cada layer. Dim(Di) = D+1
// Para Di[0]=Di[0] real + 1 (bias con entrada fija -1)
int *Di;

unsigned int timeseed;

int forceBreak = 0;

/**
 * init
 *
 * Inicializacion de las estructuras de datos para simular la red.
 *
 * W es un puntero a la matriz W.  Esta matriz tiene por cada layer
 * los pesos sinapticos de todas las conexiones posibles entre layers.
 *
 * W				Puntero a la matriz W.
 * E				Puntero a las entradas y salidas de toda la red.
 **/
void
init (weight *** W, neuron *** E)
{
    int i = 0;

    // D la cantidad de capas con neuronas (sin contar la entrada).
    *W = (weight **) malloc (sizeof (weight *) * D);
    *E = (neuron **) malloc (sizeof (weight *) * (D + 1));

    if (*W == NULL || *E == NULL)
    {
        printf ("No hay suficiente memoria.\n");
        exit (-1);
    }

    // Inicializacion de los vectores de salidas de cada capa.
    // Cada layer es de tamanio Di[i]
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


void initDW(weight ***DW)
{
    // D la cantidad de capas con neuronas (sin contar la entrada).
    *DW = (weight **) malloc (sizeof (weight *) * D);

    if (*DW == NULL)
    {
        printf ("No hay suficiente memoria.\n");
        exit (-1);
    }


    // Inicializacion matriz con los pesos sinapticos para todas las capas.
    for (int i = 0; i < D; i++)
    {
        (*DW)[i] =
                (weight *) malloc (sizeof (weight) *
                                   (Di[i] * Di[i + 1]));
        if ((*DW)[i] == NULL)
        {
            printf ("No hay suficiente memoria.\n");
            exit (-1);
        }

        memset((*DW)[i], 0, sizeof (weight)*(Di[i] * Di[i + 1]));
    }
}

void freeDW(weight ***DW)
{
    // Inicializacion matriz con los pesos sinapticos para todas las capas.
    for (int i = 0; i < D; i++)
    {
        //free((*DW)[i]);
    }

    //free( DW );
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

    printf ("Reading file...\n");

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
        // Di[0]-1 son la cantidad de entradas disponibles sobre la red.
        // Di[D] son la cantidad de neuronas en la capa final, por ende, la dimension de salida.
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
        for (j = 0; j < Di[0] - 1; j++)
        {
            sprintf (aux1, "pattern.in.%d.%d", p, j);
            getValue (buffer, aux1, filename);
            // TODO: Verificar los tipos de datos segun el problema a resolver
            //X[p][j]=atoi(buffer);
            X[p][j] = (neuron) atof (buffer);
        }

        for (j = 0; j < Di[D]; j++)
        {
            sprintf (aux1, "pattern.out.%d.%d", p, j);
            getValue (buffer, aux1, filename);
            // TODO: Verificar los tipos de datos segun el problema a resolver.
            //Y[p][j]=atoi(buffer);
            Y[p][j] = (neuron) atof (buffer);
        }

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

    RMS_BREAK = 0.1;
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
        // +1 por el bias.
        if (k == 0)
            Di[k]++;
    }

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
                *(W[k] + Di[k + 1] * i + j) =  getNaturalMinMaxProb (-1, 1) * 0.1;
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
 * Genera la matriz con las retropropagaciones de los errores para las
 * actualizaciones de los pesos sinapticos en los hidden layers.
 *
 * El calculo esta hecho para tanh()
 *
 * Li             Vector con las retropropagaciones
 * W              Matriz de pesos sinapticos
 * E              Valores de salida de las neuronas de todas las capas
 * Y              Valores de salida deseados.
 *
 **/
int
getLi (neuron ** Li, weight ** W, neuron ** E, neuron * Y)
{
    int i, j, k;
    neuron aux = 0;

    for (k = D; k > 0; k--)
    {
        if (k == D)
        {
            // Para el ultimo layer
            for (i = 0; i < Di[k]; i++)
            {
                Li[(k - 1)][i] =
                        LI_E * (1 -
                                E[k][i] * E[k][i]) * (Y[i] -
                                                      E[k]
                                                      [i]);
            }
        }
        else
        {
            // Para el k-layer
            for (i = 0; i < Di[k]; i++)
            {
                for (j = 0; j < Di[k + 1]; j++)
                {
                    aux += (Li[(k - 1) + 1][j] *
                            (*(W[k] + Di[k] * i + j)));
                }
                Li[(k - 1)][i] =
                        LI_E * (1 - E[k][i] * E[k][i]) * aux;
            }
        }
    }
    return 1;
}


/**
 * Ajusta los pesos sinapticos segun el algoritmo de backpropagation
 *
 * W                 Matriz de pesos sinapticos
 * E                 Valores de salida de todas las neuronas de todas las capas
 * Y                 Valores de salida finales deseados
 *
 **/
int
learn_backprop (weight ** W, neuron ** E, weight ** DW, neuron * Y)
{
    weight dW;
    neuron **Li;
    int k, j, i;
    int bLearn = 0;

    // Estructura para contener los valores de retropropagacion del error
    Li = (neuron **) malloc (sizeof (neuron *) * (D));

    for (k = D; k > 0; k--)
    {
        *(Li + (k - 1)) = (neuron *) malloc (sizeof (neuron) * Di[k]);

        memset(*(Li + (k - 1)),0,sizeof (neuron) * Di[k]);
    }

    // Evoluciona la red
    evolveLayeredNN (W, E);

    // Obtiene las retropropagaciones de los errores
    getLi (Li, W, E, Y);

    // Aplica la delta rule sobre todos los pesos sinapticos de toda la red
    for (k = D; k > 0; k--)
    {
        for (i = 0; i < Di[k]; i++)
        {
            for (j = 0; j < Di[k - 1]; j++)
            {
                // @TODO: we are missing the momentum value
                dW = (weight) ((DELTA_WEIGHT) *
                               Li[(k) - 1][i] * E[k - 1][j]);

                // Incrementando ACCURACY haces que la red sea mas laxa en la condicion de terminacion.
                if (fabs (dW) > 0.001)
                {
                    bLearn = 1;
                    // Si bLearn true, entonces la red sigue intentando aprender.
                    // @FIXME El valor suele ser muy chico si la dimension es mayor a 1
                }

                //*(W[k - 1] + Di[k - 1] * i + j) += dW;

                *(W[k - 1] + Di[k - 1] * i + j) += (dW + MOMENTUM * (*(DW[k - 1] + Di[k - 1] * i + j)));
                *(DW[k - 1] + Di[k - 1] * i + j) = (dW + MOMENTUM * (*(DW[k - 1] + Di[k - 1] * i + j)));

            }
        }
    }

    // Libera el espacio utilizado para almacenar los valores de errores.
    for (k = D; k > 0; k--)
    {
        free (*(Li + (k - 1)));
    }

    free (Li);

    return bLearn;
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
    float fQErr=0;

    for (p = 0; p < patternSize; p++)
    {
        E[0][0] = -1;
        for (j = 1; j < Di[0]; j++)
        {
            E[0][j] = X[p][j - 1];
        }
        evolveLayeredNN (W, E);
        for (j=0;j<Di[D];j++)
        {
            fQErr += (pow((Y[p][j] - E[D][j]),2));
        }

    }

    fQErr = fQErr / (float) patternSize;
    return fQErr;
}

float
logQuadraticError (weight ** W, neuron ** E, neuron ** X, neuron ** Y,
                   int patternSize)
{
    float rms=getQuadraticError (W, E, X, Y, patternSize);
    if (isLogging ())
    {
        sprintf (logBuffer, "%12.10f\n",
                 rms);
        logInfo (logBuffer);
    }
    return rms;
}

/**
 * learnAll
 *
 * Obtiene aleatoriamente los patrones {X,Y} y para cada uno realiza los ajustes
 * necesarios sobre los pesos sinapticos para aprenderlos.
 * La primera entrada de la red es el bias (-1) y para el resto copia los valores
 * del patron X.  Si el patron no implica ningun cambio en los pesos sinapticos
 * incrementa un contador y despues de REPLY_FACTOR intentos el algoritmo supone
 * convergencia y finaliza.
 *
 * W				Matriz de pesos sinapticos
 * E				Salidas de cada neurona de cada layer.
 * X				Patrones de entrada
 * Y				Valores deseados para cada patron.
 * patternSize Cantidad de patrones en el trainning set.
 *
 **/
void
learnAll (weight ** W, neuron ** E, neuron ** X, neuron ** Y, int patternSize)
{
    int bLearn = 0;
    int iChance;
    int j;
    unsigned long iMUpdate = 0;
    int *bLearnVector;
    float rms=1;
    weight **DW = NULL;

    bLearnVector = (int *) malloc (sizeof (int) * patternSize);

    // @FIXME memset
    for (j = 0; j < patternSize; j++)
        bLearnVector[j] = 0;

    // Log info
    sprintf (logBuffer,
             "Error cuadratico medio cada %d actualizaciones.\n",
             patternSize);
    logInfo (logBuffer);

    initDW(&DW);

    while (bLearn < REPLY_FACTOR && (forceBreak == 0))
    {
        iMUpdate++;
        iChance = getProb (0, patternSize);
        E[0][0] = -1;
        for (j = 1; j < Di[0]; j++)
        {
            E[0][j] = X[iChance][j - 1];
        }

        // Ajustar los pesos hasta que x REPLY_FACTOR repeticiones no se perciban cambios.
        if (learn_backprop (W, E, DW, Y[iChance]) == 0)
        {
            bLearn++;
            bLearnVector[iChance]++;
        }
        // Log info solo si la salida es unidimensional
        if ((iMUpdate % patternSize) == 0)
        {
            rms = logQuadraticError (W, E, X, Y, patternSize);
            if (rms < RMS_BREAK)
                break;

            //if (rms < (0.0005 * Di[D]) ) // @TODO add me as a parameter
            //    break;
        }

        if (iMUpdate % 100000 == 0)
        {
            printf("%ld:%d\n", iMUpdate, bLearn);
        }
    }

    printf("Steps: %ld\n", iMUpdate);

    freeDW (&DW);
    free (bLearnVector);
    return;
}


// JUSTFORTEST
// Hardcoded para una sola layer (no backpropagation algorithm)
int
learn (weight ** W, neuron ** E, neuron * Y)
{
    weight dW;
    int j = 0;
    int bLearn = 0;

    evolveLayeredNN (W, E);

    for (j = 0; j < Di[0]; j++)
    {
        dW = (weight) (0.0001 * E[0][j] * (Y[0] - E[1][0]));

        if (fabs (dW) > ACCURACY)
            bLearn = 1;

        *(W[0] + Di[0] * 0 + j) += dW;
    }

    return bLearn;
}


/** 
 * Fx a aprender
 */
neuron fx (neuron x, neuron y, neuron z)
{
    return ((sin (x * M_PI + M_PI) + cos (y * M_PI + M_PI) + z) / 3);
}


/**
 * Genera por stdout la salida con los valores
 * de configuracion con los patrones a aprender
 *
 **/
void
generateTrainningSet ()
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
