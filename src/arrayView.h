#include <stdio.h>


// Formato para mostrar los resultados
#define DISPLAY_FORMAT_FLOAT "%8.6f"
#define DISPLAY_FORMAT       "%d"

#ifndef NEURONTYPE
#define NEURONTYPE
typedef float neuron;
typedef float weight;
#endif


/**
 * Muestra por stdout el vector E de dimension iSize
 *
 * E 			vector a mostrar
 * iSize		dimension
 **/
void showDNeuron(int *E, int iSize);
/**
 * Muestra por stdout el vector E de dimension iSize con elementos en coma flotante
 *
 * E			vector de floats a mostrar
 * iSize		dimension del vector
 **/
void showRNeuron(float *E, int iSize);
/**
 * Muestra la matriz de pesos sinapticos W de tamanio iMax x jMax
 *
 **/
void showWeight(float *W, int iMax, int jMax);
/**
 * Muestra las matrices de pesos sinapticos para todos los layers.
 *
 * W			Array de matrices con los pesos sinapticos para cada capa
 * Di			Vector con las dimensiones de cada layer.
 * D			Cantidad de layers de la red.
 *
 **/
void showNLWeight(float **W, int *Di, int D);
void logFile(FILE *pf, neuron *E, int iSize);

