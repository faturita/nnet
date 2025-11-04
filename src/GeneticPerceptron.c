/**

Dado un perceptron multicapa:

En principio podria ser cualquiera pero lo llevo a 

2 entradas + 1 fija por el bias
1 neurona en la capa oculta

feed-forward perceptron.

Con esto tengo una poblacion de 10 
cada elemento tiene 192 bits.
Todos los elementos representan todos los pesos sinapcticos de la red

1 - Encontrar un funcional que permita ajustar los pesos para resolver un XOR de dos entradas.
2 - Encontrar un funcional que sirva tambien para resolver simulated annealing.
3 - Hacer el algoritmo.


**/

#include "layeredNNet.c"

FILE *pfLogFile;



weight
fittest(weight w)
{
	return w*w+2;
}


weight mutation(weight f1, int s)
{
	char *cf1,*cf2;
	char d[4];
	weight *res;
	int i;
	
	

	//In[0] = ( (0xFFFF << (n-s)) && I[i2]) +  ( (0xFFFF >> s) && I[i1]);
	
	cf1 = &f1;
	
	for(i=0;i<4;i++) {
		printf ("%d-",cf1[i]);
	}
	
	printf ("\n");
	
	
	for(i=0;i<s/8;i++) {
		d[i] = cf1[i];
	}
	
	// Altero el que corresponde.
	d[s/8] = ((char)pow(2,s)) ^ (  ((0xFF << (8-s%8)) & cf1[s/8]) + ((0xFF >> (s%8)) & cf1[s/8])  );
		
	for(i=s/8+1;i<sizeof(weight);i++) {
		d[i] = cf2[i];
	}
	
	//d[0] = cf1[0] ; d[1] = cf1[1] ; d[2] = cf1[2]; d[3] = cf1[3];
	//d[0] = 11 ; d[1] = 111 ; d[2] = 111; d[3] = 11;
	
	printf ("\n");
	
	for(i=0;i<4;i++) {
		printf ("%d-",d[i]);
	}
	

	res = d;
	
	
	printf ("Mutando %d  %14.10f Result %14.10f\n", s, f1,*(res));
	
	return *(res);
	
}






weight crossover(weight f1, weight f2, int s)
{
	char *cf1,*cf2;
	char d[4];
	weight *res;
	int i;


	printf ("Tamanios : int %ld, float %ld, long %ld\n",sizeof(int),sizeof(float),sizeof(double));

	//In[0] = ( (0xFFFF << (n-s)) && I[i2]) +  ( (0xFFFF >> s) && I[i1]);
	
	cf1 = &f1;
	cf2 = &f2;
	
	for(i=0;i<4;i++) {
		printf ("%d-",cf1[i]);
	}
	
	printf ("\n");
	
	for(i=0;i<4;i++) {
		printf ("%d-",cf2[i]);
	}
	
	
	for(i=0;i<s/8;i++) {
		d[i] = cf1[i];
	}
	
	// Altero el que corresponde.
	d[s/8] = ((0xFF << (8-s%8)) & cf1[s/8]) + ((0xFF >> (s%8)) & cf2[s/8]);
		
	for(i=s/8+1;i<sizeof(weight);i++) {
		d[i] = cf2[i];
	}
	
	//d[0] = cf1[0] ; d[1] = cf1[1] ; d[2] = cf1[2]; d[3] = cf1[3];
	//d[0] = 11 ; d[1] = 111 ; d[2] = 111; d[3] = 11;
	
	printf ("\n");
	
	for(i=0;i<4;i++) {
		printf ("%d-",d[i]);
	}
	
	

	res = d;
	
	
	printf ("Cortando %d  %14.10f I2 %14.10f  Result %14.10f\n", s, f1,f2,*(res));
	
	return *(res);
	
}


int 
chooseBestElement( weight *I, int M )
{
	int i;
	int iMax=0;
	
	for(i=0;i<M;i++) {
		printf ("%d - %14.10f\n", i,fittest(I[i]));
		if ( fittest(I[iMax]) > fittest(I[i]) ) {
			iMax = i;
		}
	}
	
	printf ("Max %d \n", iMax);
	return iMax;
}

int
pickElement(weight *I, int M) 
{
	float r;
	int Pac[M];
	int i;
	
	float Sum;
	float Sum2;
	float Sum3;
	
	r = getNaturalProb();
	
	Sum = 0;
	for(i = 0;i<M;i++) {
		Sum += fittest(I[i]);
	}	
	

	Sum2 = 0;
	for(i = 0;i<M;i++) {
		Sum3 = Sum2 + (fittest(I[i])/ Sum);
		printf ("Acum %14.10f -- %14.10f \n", Sum2, Sum3);
		if ( r > Sum2 && r <= (Sum3) ) break;
		Sum2=Sum3;
	}

	printf ("r %14.10f ----------------- %d\n",r,i);
	
	return i;
}



int
main (int argc, char *argv[]) 
{
	weight **W;
	char buffer[256];

	int i, j,iChance,k;

	//config ("geneticperceptron.conf");

	//init (&W, &E);
	
	// Cantidad de especimenes (cantidad de cromosomas)
	int M=10;
	
	//  Tamanio de cada cromosoma
	int n=32;
	
	int r,s;
	
	int i1,i2;
	
	
	int iBestElement;
	

	// En I tengo los valores de los comosomas actuales, en In de la nueva generacion.
	weight *I, *In;
	

    initRandom (0);
	
	
	I = (weight *)malloc(sizeof(weight)*M);
	
	In = (weight *)malloc(sizeof(weight)*M);
	
		
	//getRandomWeight (W);
	for (i=0;i<M;i++) {
		I[i] = getNaturalMinMaxProb(-10,10);
	}

	//crossover(I[0],I[1],32);
	
	iBestElement = chooseBestElement( I , M);
		
	while ( fabs( fittest( I[iBestElement]) - 2 ) > 0.001 ) {
		
		for(k=0;k<(M/2);k++) {
		
			int i1, i2;
		
			i1 = pickElement( I, M );
			i2 = pickElement( I, M );
			
			s = getProb(0,n+1);
					
			In[k*2] = crossover(I[i1],I[i2],s);
			In[k*2+1] = crossover(I[i2],I[i1],n-s);
		}
		
		k = getProb(0,M);
		s = getProb(0,n+32);
		
		In[k] = mutation(I[k],s);
		
		// The new population is now the population...
		for(i=0;i<M;i++) {
			I[i] = In[i];
		}
		
		// Selecciono el mejor elemento de la poblacion
		iBestElement = chooseBestElement( I , M );
		
	}
	
	
	printf ("Encontrado :%f\n", fittest(I[iBestElement]));
	
	return 0;	
	
}
