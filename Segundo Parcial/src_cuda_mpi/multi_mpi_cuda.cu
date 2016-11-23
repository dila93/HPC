#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include "extern.h"

__global__ void mult_mat(double *d_a, double *d_b, double *d_c, int height, int width_a, int width_b) {
  int row = blockIdx.y * blockDim.y + threadIdx.y;
  int col = blockIdx.x * blockDim.x + threadIdx.x;

  if (row < height && col < width_b) {
    double p_result = 0;
    for (int k = 0; k < width_a; k++) {
      p_result += d_a[row * width_a + k] * d_b[k * width_b + col];
    }
    d_c[row * width_b + col] = p_result;
  }
}

void mult_mat_CUDA(double *h_matriza, double *h_matrizb, double *h_matrizc, int height, int width_a, int width_b) {
  int blocksize = 32;
  double *d_a, *d_b, *d_c;

  // Asign memory in the device
  cudaMalloc(&d_a, sizeof(double) * height * width_a);
  cudaMalloc(&d_b, sizeof(double) * width_a * width_b);
  cudaMalloc(&d_c, sizeof(double) * height * width_b);

  cudaMemcpy(d_a, h_matriza, height * width_a * sizeof(double), cudaMemcpyHostToDevice);
  cudaMemcpy(d_b, h_matrizb, width_a * width_b * sizeof(double), cudaMemcpyHostToDevice);

  dim3 dimBlock(blocksize, blocksize, 1);
  dim3 dimGrid((height / blocksize) + 1, (width_b / blocksize) + 1);

  mult_mat<<< dimGrid, dimBlock >>>(d_a, d_b, d_c, height, width_a, width_b);
  cudaMemcpy(h_matrizc, d_c, height * width_b * sizeof(double), cudaMemcpyDeviceToHost);

  cudaFree(d_a);
  cudaFree(d_b);
  cudaFree(d_c);
}

void init_buf(double *matriz, int y, int x) {
  double n = 1;
  for (int i = 0; i < y; i++) {
    for (int j = 0; j < x; j++) {
      matriz[i * x + j] = n++;
    }
  }
}

bool compare_resultado(double *matriz_MPI, double *matriz_CUDA, int y, int x) {
  for (int i = 0; i < y; i++) {
    for (int j = 0; j < x; j++) {
      if (matriz_MPI[i * x + j] != matriz_CUDA[i * x + j])
        return false;
    }
  }
  return true;
}