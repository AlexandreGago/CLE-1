mpicc -Wall -O3 -g -o ex2 ex2.c merge.c
rm -f ex2.80s*

# mpirun -n $1 ./ex2 $2 xterm -e gdb ./ex2

mpirun -np $1 xterm -e gdb -x commands.txt --args ./ex2 $2


