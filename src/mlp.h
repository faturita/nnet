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

void forward(weight ** W, neuron ** E);
void back(neuron **Li, weight ** W, neuron ** E, neuron * Y);
void allocate (weight *** W, weight *** dW, neuron *** E, neuron *** Li);
