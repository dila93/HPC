#!/bin/bash

#SBATCH --job-name=mult_matrix
#SBATCH --output=res_mpi_mult_matrix.out
#SBATCH --nodes 1
#SBATCH --ntasks-per-node 8
#sbatch --gres=gpu:1

/usr/local/bin/mpicxx -o multi.o -c multi_mpi.c

"/usr/local/cuda-8.0"/bin/nvcc -ccbin g++ -m64 -gencode arch=compute_30,code=sm_30 -gencode arch=compute_35,code=sm_35 -gencode arch=compute_37,code=sm_37 -ge$

/usr/local/bin/mpicxx -o multi multi.o multi_cuda.o  -L"/usr/local/cuda-8.0"/lib64 -lcudart

mpirun multi
