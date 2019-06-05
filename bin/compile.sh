gcc ../src/parameter.c ../src/gridParser.c ../src/HopfieldNet.c ../src/mathNeuron.c -o HopfieldNet /usr/lib/libm.so
gcc ../src/mathNeuron.c ../src/Ising.c -o Ising /usr/lib/libm.so
gcc ../src/parameter.c ../src/gridParser.c ../src/HopfieldNet84.c ../src/mathNeuron.c -o HopfieldNet84 /usr/lib/libm.so 
gcc ../src/parameter.c ../src/Perceptron.c ../src/arrayView.c ../src/mathNeuron.c ../src/logger.c -o Perceptron /usr/lib/libm.so
gcc ../src/parameter.c ../src/Kohonen.c    ../src/arrayView.c ../src/mathNeuron.c ../src/logger.c -o Kohonen /usr/lib/libm.so
gcc -g -Wall ../src/parameter.c ../src/showme.c -o showme -I/usr/local/include/gtk-1.2 -I/usr/local/lib/glib/include -I/usr/local/include -I/usr/X11R6/include -L/usr/local/lib -L/usr/X11R6/lib -lgtk -lgdk -rdynamic -lgmodule -lglib -ldl -lXext -lX11 -lm
