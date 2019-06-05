/**
 * Rutinas para validacion, testing, generacion de redes neuronales tipo Hopfield 82.
 *
 */

#include "neuron84.c"
#include "gridParser.h"


/*
 * Verifica la capacidad de la red.
 *
 * Parametros:
 *		pfLogFile		Archivo de log.
 *
 * Salida:
 *		archivo pfLogFile	(cantidad de neuronas, palabras aprendidas)
 *
 */
/*int TestCapacity(FILE *pfLogFile)
{
    float *W;
    neuron *E;
    neuron *Y;
    neuron *EE;
    int s;
    neuron iCapacity;

    char msgBuffer[256];

    for(N=N_MIN;N<N_MAX;N+=N_INTERVAL) {
	for(P=1;P<N;P+=1) {
	    init (&W, &EE, &E, &Y);
	    setUpNet(W, EE);
	    iCapacity = P;
	    for (s=0;s<P;s++) {
		copyMemory(Y, EE+N*s);
		evolveNet(W, Y);
		if ( !equal( Y, EE+N*s) ) {
		    iCapacity--;
		}
	    }
	    if (iCapacity<P)
		break;
	}
	sprintf (msgBuffer, "%d,%d\n", N, iCapacity);
	fputs(msgBuffer, pfLogFile);
	fflush(pfLogFile);
    }

} */

/*
 * Genera un log con la relacion entre conexiones perdidas y capacidad de la red.
 * Las conexiones se van eliminando de a una al azar.
 *
 * Parametros:
 *		pfLogFile		Archivo de log.
 *
 * Salida:
 *		Archivo pfLogFile	(% conexiones perdidas, % capacidad remanente)
 */
/*int TestLostConnections(FILE *pfLogFile)
{
    float *W;
    neuron *E;
    neuron *Y;
    neuron *EE;
    int s;
    int iLC;
    char msgBuffer[256];
    int iTotalCon = N * N;

    int iLost;

    init (&W, &EE, &E, &Y);
    setUpNet(W, EE);

    for(iLC=0;iLC<iTotalCon;iLC+=iTotalCon/100) {
	iLost = P;
	alterWeight(W,iTotalCon/100);
	for(s=0;s<P;s++) {
	    copyMemory(Y,EE+N*s);
	    evolveNet(W,Y);
	    if ( !equal( Y, EE+N*s )) {
		iLost--;
	    }
	}
	sprintf (msgBuffer,"%d,%d\n",(iLC*100)/iTotalCon,(iLost*100)/P);
	fputs(msgBuffer, pfLogFile);
	fflush(pfLogFile);
    }

} */

/*
 * Verifica el funcionamiento de la red, mostrando por pantalla los resultados
 * intermedios obtenidos.
 */
int TestNetWork()
{
    float *W;			// Matriz de pesos sinapticos
    neuron *E;			// Dato auxiliar.
    neuron *Y;			// Salida de la red.
    neuron *EE;			// Matriz de datos a almacenar
    int s;			// Indice de memorias.

    // Inicializa memoria
    init(&W,&EE,&E,&Y);

    // Inicializa la red (patrones pseudoaleatorios segun parametros de conf.)
    setUpNet(W,EE);//showWeight(W,N,N);

    // Obtiene una memoria al azar.
    getMemory(Y);copyMemory(Y, EE);

    // Resguarda el dato en un buffer auxiliar.
    copyMemory(E,Y);

    // Evolucion de la red.
    evolveNet(W, Y);

    // Se muestran las memorias generadas
    printf ("Memorias memorizadas:\n");
    for (s=0;s<P;s++) {
        showMemory(EE+N*s);
        printf ("\n");
    }

    //showWeight(W, N, N);

    printf ("\nDato a recuperar:\n");showMemory( E );

    printf ("\nDato recuperado:\n");showMemory( Y );printf ("\n");


    copyMemory(Y, EE+2);

    evolveNet(W, Y);
    printf ("\nDato a recuperar:\n");showMemory( EE+1 );

    printf ("\nDato recuperado:\n");showMemory( Y );printf ("\n");



    return (0);
}

/*
 * Toma los parametros de los archivos de configuracion y mapea los datos
 * en memorias que son almacenadas en una red de hopfield 82.  Luego
 * verifica los datos almacenados, asociando un patron y haciendo evolucionar
 * la red.
 * Parametros:
 *	sFileGridConf		Archivo de configuracion de los datos a almacenar.
 *	sFileGridIn		Archivo con la memoria a asociar.
 * Salida:
 *	STDOUT			Representacion del dato obtenido al evolucionar la red.
 */
/*void GridMap(char *sFileGridConf, char *sFileGridIn) {
    float *W;			// Matriz de pesos sinapticos
    int *E;			// Dato auxiliar.
    int *Y;			// Salida de la red.
    int *EE;			// Matriz de datos a almacenar
    int s;

    init(&W,&EE,&E,&Y);

    // Se graban los patrones a memorizar.
    for (s=0;s<P;s++) {
        Y = getGridMemory(sFileGridConf,s);
        copyMemory(EE+s*N,Y);
    }

    // Se genera la matriz con los pesos sinapticos.
    getWeight(W, EE);

    // Se carga el patron a verificar.
    E = getGridMemory(sFileGridIn,0);

    // Se hace evolucionar la red, con lo que se modifica el dato ingresado.
    evolveNet( W , E);

    //showMemory(E);
    showGridMemory(E);

} */

int main(int argc, char *argv[])
{
    FILE *pfLogFile;
    int iOpcion;

    printf ("Simulacion Red Hopfield 84 (v1.0) - 2003.\n");

    initRandom();

    getParameter(&N,&P,&N_MIN,&N_MAX,&N_INTERVAL,&LC);

    TestNetWork();exit(-1);

    if (argc != 2) {
        printf ("Opciones:\n");
        printf ("1 - Comprobacion de capacidad.\n");
        printf ("2 - Comprobacion de conexiones perdidas sobre capacidad.\n");
        printf ("3 - Verificacion de estados espureos.\n");
        printf ("4 - Busqueda asociativa sobre grillas.\n");
        printf ("5 - Generacion de Patrones en Secuencia Temporal. \n\n");
        exit(-1);
    }

    if ((pfLogFile = fopen("hopnet.log","w+"))==NULL) {
        printf ("ERROR:  No es posible abir archivo de log.\n");
        exit(-1);
    }

    iOpcion = atoi(argv[1]);
    switch (iOpcion) {
    case 5:
        printf ("Grilla con los datos a memorizar:  [grid.data.conf].\n");
        printf ("Dato a asociar para la red: [grid.in]. \n");
        printf ("Generando secuencia temporal de patrones. \n");
        //verifyBAM("grid.data.conf","grid.in");
        break;
    case 4:
        printf ("Grilla con los datos a memorizar:  [grid.data.conf].\n");
        printf ("Dato a asociar para la red: [grid.in]. \n");
        //GridMap("grid.data.conf","grid.in");
        break;
    case 3:
        printf ("Comprobacion de datos espureos.\n");
        printf ("grid.spurious.conf -  Contiene datos no pseudoortogonales.\n");
        printf ("grid.spurious.in   -  Contiene un dato a asociar.\n");
        printf ("La salida de este set de datos es un valor de energia minima y no es ninguno de los datos ingresados (dato espureo).\n");
        //GridMap("grid.spurious.conf","grid.spurious.in");
        break;
    case 2:
        printf ("Generando salida (conexiones rotas, capacidad remanente) en archivo hopnet.log.\n");
        //TestLostConnections(pfLogFile);
        break;
    case 1:
        printf ("Generando salida (tamanio red,memorias) en archivo hopnet.log.\n");
        //TestCapacity(pfLogFile);
        break;
    default:
        printf ("Opcion no valida.\n");
        break;
    }

    fclose(pfLogFile);

}
