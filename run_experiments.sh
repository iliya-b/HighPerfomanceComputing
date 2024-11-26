#!/bin/bash

function run_matrix {
    echo -n "3000,$OMP_NUM_THREADS,"
    echo 3000 | ./matrix > /dev/null
    echo -n "6000,$OMP_NUM_THREADS,"
    echo 6000 | ./matrix > /dev/null
    echo -n "8000,$OMP_NUM_THREADS,"
    echo 8000 | ./matrix > /dev/null
    echo -n "10000,$OMP_NUM_THREADS,"
    echo 10000 | ./matrix > /dev/null
}

g++ -fopenmp lab_parallel.cpp -o matrix -DOMP=1

export OMP_NUM_THREADS=1
run_matrix

export OMP_NUM_THREADS=2
run_matrix

export OMP_NUM_THREADS=3
run_matrix

export OMP_NUM_THREADS=4
run_matrix

export OMP_NUM_THREADS=5
run_matrix

export OMP_NUM_THREADS=6
run_matrix


export OMP_NUM_THREADS=7
run_matrix


export OMP_NUM_THREADS=8
run_matrix



export OMP_NUM_THREADS=9
run_matrix

export OMP_NUM_THREADS=10
run_matrix


export OMP_NUM_THREADS=11
run_matrix


export OMP_NUM_THREADS=12
run_matrix