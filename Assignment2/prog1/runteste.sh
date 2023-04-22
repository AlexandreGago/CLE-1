mpicc -Wall -O3 -o teste teste.c textProcessing.c
rm -f teste.80s*

mpirun -n $1 ./teste $2 $3 $4 $5 $6 $7 $8 $9

