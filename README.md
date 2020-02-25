# NNet
Deep Neural Networks from scratch
Featuring Hopfield 82 and Ising Model

Resumen:

Lenguaje: ANSI-C
Plataforma: IBM-PC i586 Linux Mandrake 7.1
Gcc: gcc version 2.95.3 19991030 (prerelease)

Sinapsis:

./bin/HopfieldNet [opcion]


1	-	Comprobaci�n de capacidad

	Obtiene los par�metros de parameter.conf, ejecuta calculando la capacidad
	de la red y genera el archivo de salida "hopnet.log" con la informaci�n 
	generada.

2	-	Comprobaci�n de conexiones perdidas sobre capacidad.
	
	Genera la red segun los par�metros de parameter.conf, y comienza a eliminar
	conexiones aleatoriamente, registrando la cantidad de memorias almacenadas
	correctamente en funcion del porcentage de las conexiones eliminadas	

3	-	Verificaci�n de estados esp�reos.

	Genera memorias en base a los par�metros de parameter.conf y de las grillas
	grid.spurious.conf con los patrones a almacenar y grid.spurious.in con el 
	patron de entrada para la red.  Por la configuraci�n de los patrones, se 
	la red converge a una memoria espuria.

4	-	Busqueda asociativa sobre grillas.

	Genera la red en base a los patrones a almacenar contenidos en grid.data.conf
	e ingresa a la red el patron grid.in, convergiendo a una de las memorias almacenadas.

5	-	Generacion de patrones en secuencia temporal.
	
	Carga los patrones en grid.data.conf correlacionados entre s� de forma que al ingresar uno
	como entrada converga al minimo que corresponde con el otro patron.  Esto esta implementado
	solo para procesar 3 patrones en grilla con el orden 1->0->2->1.


Consulte ./bin/parameter.conf  para verificar como configurar la red.


# Ising
./bin/Ising

Ejecuta 10 simulaciones del Modelo de Ising obteniendo el valor correspondiente
a la temperatura critica, como promedio de los valores donde se obtiene el 90 % 
de la magnetizacion. Los parametros utilizados en la simulacion son los siguientes:

* J:Interaccion entre spines = 1
* H:Magnetizacion externa    = 1
* k:Constante de Boltzmann   = 1
* T:Temperatura inicial      = 10


La salida la genera en el archivo "ising.log"


# Dumps:

En ./dumps/ se encuentran las salidas de los procesos ejecutados:

* hopnet.200.10.log		Salida comprobacion conexiones perdidas en una red de 200 neuronas y 10 patrones
* hopnet.300.30.log		Salida comprobaci�n conexiones perdidas en una red de 300 neuronas y 30 patrones
* hopnet.400.50.log		Salida comprobaci�n conexiones perdidas enuna red de 400 neuronas y 50 patrones
* hopnet.capacity.log		Salida comprobaci�n capacidad de la red.
* ising.log                       Salida obtenci�n Temperatura Cr�tica




