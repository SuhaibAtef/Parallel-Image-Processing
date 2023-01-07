mpic++ -O3 -o MainE mainE.cpp -lm -lpthread -lX11
mpirun -np 4 ./MainE
