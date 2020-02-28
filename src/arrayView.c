#include <stdio.h>
#include "arrayView.h"

// Screen Functions ------------------------------------------------
void showWeight(float *W, int iMax, int jMax) {
	int i;
   int j;

   for (i=0;i<iMax;i++) { 			// Neuron counter
   	printf ("[");
   	for (j=0;j<jMax;j++) {     // Input counter
      	printf ("%8.6f",*(W+i*jMax+j));
         if (j+1<jMax) printf (";");
      }
      printf ("]\n");
   }
   
}

void showNLWeight(float **W, int *Di, int D) {
	int i,k,j;
   
   for (k=0;k<D;k++) {
   	printf ("Layer: %d M (%d,%d)\n",k,Di[k],Di[k+1]);
      showWeight(W[k],Di[k],Di[k+1]);
   }
}

/*
 * Muestra por pantalla la memoria E
 */
void showDNeuron(int *E,int iSize)
{
   int i;
   printf ("[");

   for (i=0;i<iSize;i++) {
   	printf (DISPLAY_FORMAT,*(E+i));
      if (i+1<iSize)
      	printf (";");
   }
   printf ("]");

}

void logFile(FILE* pf, neuron *E, int iSize)
{
    if (!pf) return;

    for (int i=0;i<iSize;i++) {
       fprintf(pf,"%10.6f",*(E+i));
       if (i+1<iSize)
         fprintf (pf,",");
    }
    fprintf(pf,"\n");
    fflush(pf);
}

/*
 * Muestra por pantalla la memoria E
 */
void showRNeuron(float *E,int iSize)
{
   int i;
   printf ("[");

   for (i=0;i<iSize;i++) {
   	printf ("%10.6f",*(E+i));
      if (i+1<iSize)
      	printf (";");
   }
   printf ("]");

}

// ------------------------------------------------------------------
