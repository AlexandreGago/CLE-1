#!/bin/bash

export GMON_OUT_PREFIX=gmon.out-


mpicc -Wall -pg -O3 -o   ex1 ex1.c textProcessing.c
rm -f ex1.80s*
rm -f gmon.out*

mpiexec -n 9 ./ex1 -s 4 dataSet1/text0.txt dataSet1/text1.txt dataSet1/text2.txt dataSet1/text3.txt dataSet1/text4.txt

gprof -s ./ex1 gmon.out-*
gprof ./ex1 gmon.sum
