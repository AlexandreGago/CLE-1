#!/bin/bash
mpicc -Wall -O3 -o ex1 main.c textProcessing.c
rm -f ex1.80s*



for n in 2 3 5 9
do
    # run the command 10 times
    for i in {1..10}
    do
        mpirun -n $n ./ex1 -s 8 dataSet1/text0.txt dataSet1/text1.txt dataSet1/text2.txt dataSet1/text3.txt dataSet1/text4.txt
    done
    # print an empty line to results.txt
    echo "" >> results.txt
done
