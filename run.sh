mpic++ -O3 -o Main main.cpp -lm -lpthread -lX11
mpirun -np 2 Main
