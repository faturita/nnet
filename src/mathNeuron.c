#include <stdio.h>
#include "mathNeuron.h"

/*
 * Devuelve 1 o  -1.
 * 
 **/
int getSpinProb() {
	return ( 2* getProb(0,1) - 1);
}

/*
 * Devuelve un numero al azar entre iMin e iMax.
 */
int getProb(int iMin, int iMax)
{
    return (iMin+(int)(iMax*1.0*rand()/(RAND_MAX+1.0)));
}

/**
 * P() constante entre 0 y 1
 */
float getNaturalProb()
{
    return ((float)(1.0*rand()/(RAND_MAX+1.0)));
}

/**
 * P() contante entre iMin y iMax
 */
float getNaturalMinMaxProb(float iMin, float iMax)
{
	return (iMin+(float)((iMax-iMin)*1.0*rand()/(RAND_MAX+1.0)));
}
/*
 * Devuelve el signo de fVal.
 */
int sgn(float fVal)
{
	if (fVal < 0)
		return (-1);
	else
		return (1);
}

/**
 * Devuelve el valor de la tangente hiperbolica
 */
float tanhsigmoid(float fVal) {
	return ( (exp( fVal ) - exp ( -fVal ))/(exp(fVal) + exp(-fVal)));
}

/**
 * @brief thetanhsigmoid LeCun, "Efficient Backprop", 1998  Ideal cuando las salidas son binarias, ya que el maximo de la derivada esta en -1 y +1
 * @param fVal
 * @return
 */
float thetanhsigmoid(float fVal) {
    printf ("%f\n", fVal);
    float val = (1.7159*tanhsigmoid(2.0/3.0 * fVal));
    assert(!isnan(val) && "Error");
    return val;
}

/**
 * Devuelve el valor de la exponencial
 */
float expsigmoid(float fVal) {
	return (  1.0 / (1 + exp( -fVal )));
}

/*
 * Inicializa las funciones de numeros al azar.
 */
unsigned int initRandom(unsigned int timeseed)
{
    time_t tim;
    time(&tim);

    if (timeseed != 0)
    {
        srand(timeseed);
        return timeseed;
    }
    else
    {
        srand((unsigned int)(tim));
        return (unsigned int)(tim);
    }

    return (unsigned int)(tim);
}
