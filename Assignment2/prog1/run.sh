mpicc -Wall -O3 -o ex1 ex1.c textProcessing.c
mpirun -n $1 ./ex1 $2 $3 $4 $5 $6 $7 $8 $9