#!/bin/bash

export GMON_OUT_PREFIX=gmon.out-


mpicc -Wall -pg -O3 -g -o ex2 main.c merge.c
rm -f ex2.80s*
rm -f gmon.out*

mpirun -n $1 ./ex2 $2

gprof -s ./ex2 gmon.out-*
gprof ./ex2 gmon.sum
