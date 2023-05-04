mpicc -Wall -O3 -o ex1 ex1.c textProcessing.c
rm -f ex1.80s*

# mpirun -n $(($1+1)) ./ex1 $2 $3 $4 $5 $6 $7 $8 $9

mpirun -n 2 ./ex1 $1 $2 $3 $4 $5 
mpirun -n 3 ./ex1 $1 $2 $3 $4 $5
mpirun -n 5 ./ex1 $1 $2 $3 $4 $5
mpirun -n 9 ./ex1 $1 $2 $3 $4 $5
