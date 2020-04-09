C implementation of DGSD (Distributed Graph Statistical Distances) graph representation algorithm. The implementation is based on igraph and MPI libraries therefore both the libraries should be installed to run the code. For the installation, we suggest to follow the instructions provided by the documentations of both the libraries.

run the code with the following commands

compilation: mpicc -o dgsd -g dgsd_main.c -I/usr/local/igraph -L/usr/local/lib -ligraph

execution: mpirun -np 10 ./dgsd   # 10 is the number of processes which can be choosen according to the environment.
