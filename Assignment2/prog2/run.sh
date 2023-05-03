mpicc -Wall -O3 -g -o ex2 ex2.c merge.c -lm
rm -f ex2.80s*

# mpirun -n $1 ./ex2 $2

mpirun -n 1 ./ex2 $2
mpirun -n 2 ./ex2 $2
mpirun -n 4 ./ex2 $2
mpirun -n 8 ./ex2 $2


# mpirun -np $1 xterm -e gdb -x commands.txt --args ./ex2 $2


