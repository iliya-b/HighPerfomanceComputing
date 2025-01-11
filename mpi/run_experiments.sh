#!/bin/bash


function run_matrix {
    echo -n "3240,$OMP_NUM_THREADS,"
    mpirun -n $OMP_NUM_THREADS ./matrix 3240 > /dev/null
    echo -n "6480,$OMP_NUM_THREADS,"
    mpirun -n $OMP_NUM_THREADS ./matrix 6480 > /dev/null
    echo -n "8640,$OMP_NUM_THREADS,"
    mpirun -n $OMP_NUM_THREADS ./matrix 8640 > /dev/null
    echo -n "10800,$OMP_NUM_THREADS,"
    mpirun -n $OMP_NUM_THREADS ./matrix 10800 > /dev/null
}

mpic++ lab_mpi.cpp -o matrix
#mpicxx-openmpi-mp

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


# export OMP_NUM_THREADS=7
# run_matrix


export OMP_NUM_THREADS=8
run_matrix


export OMP_NUM_THREADS=9
run_matrix

export OMP_NUM_THREADS=10
run_matrix


# export OMP_NUM_THREADS=11
# run_matrix


export OMP_NUM_THREADS=12
run_matrix