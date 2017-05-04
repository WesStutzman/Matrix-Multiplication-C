#!/bin/bash
# Wesley Stutzman
# Compile pthreads in c

rm -r main
clear
gcc *.c -o main -lpthread
./main
