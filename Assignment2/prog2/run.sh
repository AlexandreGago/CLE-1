mpicc -Wall -O3 -o ex2 ex2.c
rm -f ex2.80s*

mpirun -n $1 ./ex2 $2 $3 $4 $5 $6 $7 $8 $9

