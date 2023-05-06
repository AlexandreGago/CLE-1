#!/bin/bash

mpicc -Wall -O3 -g -o ex2 main.c merge.c

rm -f ex2.80s*

perf stat -e LLC-loads,LLC-load-misses mpirun -n 1 ./ex2 dataSet2/datSeq256K.bin
perf stat -e LLC-loads,LLC-load-misses mpirun -n 2 ./ex2 dataSet2/datSeq256K.bin
perf stat -e LLC-loads,LLC-load-misses mpirun -n 4 ./ex2 dataSet2/datSeq256K.bin
perf stat -e LLC-loads,LLC-load-misses mpirun -n 8 ./ex2 dataSet2/datSeq256K.bin
