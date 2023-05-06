#!/bin/bash
mpicc -Wall -O3 -g -o ex2 main.c merge.c
rm -f ex2.80s*

# mpirun -n $1 ./ex2 $2
# do this 10 times
# mpirun -n 1 ./ex2 $1

# do this 10 times for each n

files=("dataSet2/datSeq256K.bin" "dataSet2/datSeq1M.bin" "dataSet2/datSeq16M.bin")

for file in "${files[@]}"
do
    echo "# $file" >> results.txt
    echo "# $file"

    for n in 1 2 4 8
    do
        # run the command 10 times
        for i in {1..10}
        do
            mpirun -n $n ./ex2 $file
        done
        # print an empty line to results.txt
        echo "" >> results.txt
    done
done








# mpirun -np $1 xterm -e gdb -x commands.txt --args ./ex2 $2


