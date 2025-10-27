FUENTES=src/*.c
PRODUCTO=HopfieldNet
BIN=bin/
CGLAGS=-g -w -Wall -fpermissive
LIB=-lm 
all:	$(PRODUCTO)
$(PRODUCTO):	$(FUENTES)

	gcc $(CFLAGS) -o $(BIN)HopfieldNet       src/parameter.c src/gridParser.c src/HopfieldNet.c                                        src/mathNeuron.c $(LIB)
	gcc $(CFLAGS) -o $(BIN)Ising                                              src/Ising.c                                              src/mathNeuron.c $(LIB)   
	gcc $(CFLAGS) -o $(BIN)HopfieldNet84     src/parameter.c src/gridParser.c src/HopfieldNet84.c                                      src/mathNeuron.c $(LIB)
	gcc $(CFLAGS) -o $(BIN)Perceptron        src/praxis.c src/parameter.c                  src/Perceptron.c                     src/arrayView.c     src/mathNeuron.c src/logger.c $(LIB)
	gcc $(CFLAGS) -o $(BIN)GeneticPerceptron src/praxis.c src/parameter.c                  src/GeneticPerceptron.c              src/arrayView.c     src/mathNeuron.c src/logger.c $(LIB)                    
	gcc $(CFLAGS) -o $(BIN)TSPKohonen        src/commandline.c src/parameter.c                  src/TSPKohonen.c                     src/arrayView.c     src/mathNeuron.c src/logger.c $(LIB)                                           
	gcc $(CFLAGS) -o $(BIN)Kohonen           src/commandline.c src/parameter.c                  src/KohonenFeatureMapping.c          src/arrayView.c     src/mathNeuron.c src/logger.c $(LIB)
	gcc $(CFLAGS) -o $(BIN)AnnealingHopfield src/parameter.c src/gridParser.c src/AnnealingHopfield.c                                  src/mathNeuron.c $(LIB)
	gcc $(CFLAGS) -o $(BIN)Autoencoder       src/praxis.c src/parameter.c                  src/Autoencoder.c                    src/arrayView.c     src/mathNeuron.c src/logger.c $(LIB)
	gcc $(CFLAGS) -o $(BIN)nnet		         src/praxis.c src/parameter.c                  src/mlp.c           			         src/arrayView.c     src/mathNeuron.c src/logger.c  $(LIB)


Showme: $(FUENTES)
	gcc -g -Wall -o bin/showme src/showme.c src/parameter.c -lgtk -rdynamic -lgmodule -lglib -ldl -lXext -lX11 -lm -I/usr/include/gtk-1.2/ -I/usr/include/glib-1.2/ -I/usr/lib/glib/include/
	#g++ -g -Wall -o bin/showme src/showme.c src/parameter.c -I/opt/gnome/include/gtk-1.2 -I /opt/gnome/include/glib-1.2 -I /opt/gnome/include/ -I/usr/include -I/usr/X11R6/include -I/usr/include/gtk-2.0/ -L/usr/local/lib -L/usr/X11R6/lib -lgtk -lgdk -rdynamic -lgmodule -lglib -ldl -lXext -lX11 -lm -L/opt/gnome/lib

GeneticPerceptron:  $(FUENTES)
	gcc $(CFLAGS) -o $(BIN)GeneticPerceptron src/parameter.c                  src/GeneticPerceptron.c              src/arrayView.c     src/mathNeuron.c src/logger.c $(LIB)                    

TSPKohonen: $(FUENTES)
	gcc $(CFLAGS) -o $(BIN)TSPKohonen        src/parameter.c                  src/TSPKohonen.c                     src/arrayView.c     src/mathNeuron.c src/logger.c $(LIB)                                           
 
clean:
	rm -f *.o
    
clobber: clean
	rm -f $(PRODUCTO)
    
lint: $(PRODUCTO)
	lint $(FUENTES)     


#gcc ../src/parameter.c ../src/gridParser.c ../src/HopfieldNet.c ../src/mathNeuron.c -o HopfieldNet /usr/lib/libm.so
#gcc ../src/mathNeuron.c ../src/Ising.c -o Ising /usr/lib/libm.so
#gcc ../src/parameter.c ../src/gridParser.c ../src/HopfieldNet84.c ../src/mathNeuron.c -o HopfieldNet84 /usr/lib/libm.so 
#gcc ../src/parameter.c ../src/Perceptron.c ../src/arrayView.c ../src/mathNeuron.c ../src/logger.c -o Perceptron /usr/lib/libm.so
#gcc ../src/parameter.c ../src/Kohonen.c    ../src/arrayView.c ../src/mathNeuron.c ../src/logger.c -o Kohonen /usr/lib/libm.so
#gcc -g -Wall ../src/parameter.c ../src/showme.c -o showme -I/usr/local/include/gtk-1.2 -I/usr/local/lib/glib/include -I/usr/local/include -I/usr/X11R6/include -L/usr/local/lib -L/usr/X11R6/lib -lgtk -lgdk -rdynamic -lgmodule -lglib -ldl -lXext -lX11 -lm
