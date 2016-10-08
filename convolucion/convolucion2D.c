#include <stdio.h>
#include <cuda.h>
#include <stdlib.h>
#include <time.h>

//M es el tamaño del vector 1
#define M 10
//N es tanto el tamaño del vector 2 como el taaño de la mascara
#define N 3
//P es el tamaño del vector convolusionado
//P es el mismo tamaño del vector 1 (M)
#define P 3

void matrixMult_CPU_2D(int *h_matrizA2D, int *h_matrizB2D, int * h_matrizA2DC){

	for (int i = 0; i < M*N; i++) {
		for (int j = 0; j < M*N; j++) {
			double suma = 0;
			int N_start_point = i - ((N*P) / 2);
			int M_start_point = j - ((N*P) / 2);

			for (int k = 0; k < (N*P); k++) {
				for (int l = 0; l < (N*P); l++) {
					if (N_start_point + k >= 0 && N_start_point + k < (M*N) && M_start_point + l >= 0 && M_start_point + l < (M*N)) {
						suma += h_matrizA2D[(M*N) * (N_start_point + k) + (M_start_point + l)] * h_matrizB2D[(N*P) * k + l];
					}
				}
			}
			h_matrizA2DC[(M*N) * i + j] = suma;
		}
	}
}

__global__ void matrixMult_GPU_2D(int *d_matrizA2D, int *d_matrizB2D, int *d_matrizA2DC) {
	int row = blockIdx.y * blockDim.y + threadIdx.y;
	int col = blockIdx.x * blockDim.x + threadIdx.x;

	if (row < (M*N) && col < (M*N)) {
		double suma = 0;
		int N_start_point = row - ((N*P) / 2);
		int M_start_point = col - ((N*P) / 2);

		for (int i = 0; i < (N*P); i++) {
			for (int j = 0; j < (N*P); j++) {
				if (N_start_point + i >= 0 && N_start_point + i < (M*N) && M_start_point + j >= 0 && M_start_point + j < (M*N)) {
					suma += d_matrizA2D[(M*N) * (N_start_point + i) + (M_start_point + j)] * d_matrizB2D[(N*P) * i + j];
				}
			}
		}
		d_matrizA2DC[(M*N) * row + col] = suma;
	}
}


void printCPU_2D(double time_CPU2D, int *h_matrizA2D, int *h_matrizB2D, int *h_matrizA2DC){

	printf("---------------------------------------------------------------------------------------------\n");
	printf("Tiempo ejecuccion 2D CPU: %.10f\n",time_CPU2D);
	printf("Analisis 2D en CPU:\n\n");

	for (int i = 0; i < M*P; i++) {
		printf("Resultado de la multiplicación CPU: %i---%i\n",i,h_matrizA2DC[i]);
	}
}

void printGPU_2D(double time_GPU2D, int *d_matrizA2D, int *d_matrizB2D, int *d_matrizA2DC){

	printf("---------------------------------------------------------------------------------------------\n");
	printf("Tiempo ejecuccion 2D GPU: %.10f\n",time_GPU2D);
	printf("Analisis 2D en GPU:\n\n");

	for (int i = 0; i < M*P; i++) {
		printf("Resultado de la multiplicación GPU: %i---%i\n",i,d_matrizA2DC[i]);
	}
}

int main(){
  
  int *h_matrizA2D, *h_matrizB2D, *h_matrizA2DC;
  int *d_matrizA2D, *d_matrizB2D, *d_matrizA2DC;
  
  clock_t start_CPU2D, endCPU2D, start_GPU2D, endGPU2D;//Variables para tomar el tiempo de ejecuccion
  double time_CPU2D, time_GPU2D;
  
  int size1 = M * N * sizeof(int);
  int size2 = N * P * sizeof(int);
  int size3 = M * P * sizeof(int);
  //Memoria para la matriz1 CPU
  h_matrizA2D= (int*)malloc(size1);
  //Memoria para la matriz2 CPU
  h_matrizB2D=(int*)malloc(size2);
  //Memoria para la matriz3 resultado CPU
  h_matrizA2DC=(int*)malloc(size3);
  
  int cont=1;
  //inicializacion de las matriz1 CPU
	for (int j = 0; j < M; j++) {		
		for(int i=0; i<N; i++){
			h_matrizA2D[j*N+i]=rand()%4;
			cont++;
		}		
	}
	cont=1;
	//inicializacion de las matriz2 CPU
	for (int j = 0; j < N; j++) {		
		for(int i=0; i<P; i++){
			h_matrizB2D[j*P+i]=rand()%4;
			cont++;
		}		
	}
	
	//Empieza ejecuccion en la CPU
	start_CPU2D = clock();
	
	matrixMult_CPU_2D(h_matrizA2D,h_matrizB2D,h_matrizA2DC);
	
	endCPU2D = clock();

	time_CPU2D = (((double) (endCPU2D - start_CPU2D))) / CLOCKS_PER_SEC;

	printCPU_2D(time_CPU2D, h_matrizA2D, h_matrizB2D, h_matrizA2DC);
	
	
	//****inicio algoritmo GPU*****//
	start_GPU2D = clock();
	//Asignacion memoria variables para GPU
	cudaMalloc((void **)&d_matrizA2D,size1);
	cudaMalloc((void **)&d_matrizB2D,size2);
	cudaMalloc((void **)&d_matrizA2DC,size3);
	
	//copy data to divice
	cudaMemcpy(d_matrizA2D,h_matrizA2D,size1,cudaMemcpyHostToDevice);
	cudaMemcpy(d_matrizB2D,h_matrizB2D,size2,cudaMemcpyHostToDevice);
	
	//definir las dimensiones para el kernel
	int block= 4;
	dim3 dimBlock(block, block,1);//ak se defiene el numero de hilos dentro de cada block
	dim3 dimGrid(ceil(M/float(block)),ceil(M/float(block)),1);//ak se define el numero de block en mi grilla
	
	//se lanza el Kernel-- se puede lanzar el kernel multiplicacion normal * con tiled
	//solamente se debe comentar una linea y descomentar la otra para que no
	//sea pesado para el servidor
  
	matrixMult_GPU_2D<<<dimGrid,dimBlock>>>(d_matrizA2D,d_matrizB2D,d_matrizA2DC);
  
	//se copiand los datos del Device to CPU
	cudaMemcpy(h_matrizA2DC, d_matrizA2DC, size3,cudaMemcpyDeviceToHost);
	
	endGPU2D = clock();
  
  	//Se imprimen los tiempos de ejecuccion 
	time_GPU2D=(((double) (endGPU2D - start_GPU2D))) / CLOCKS_PER_SEC;
	printGPU_2D(time_GPU2D, h_matrizA2D, h_matrizB2D, h_matrizA2DC);
 
   //imprime resultado
 	/* for (int i = 0; i < M*P; i++) {
     	printf("Resultado de la multiplicación GPU: %i---%f\n",i,h_matrizA2DC[i]);
 	 }*/
  
  	free(h_matrizA2D);
  	free(h_matrizB2D);
  	free(h_matrizA2DC);

  	cudaFree(d_matrizA2D);
  	cudaFree(d_matrizB2D);
  	cudaFree(d_matrizA2DC);

  	return 0; 

}
