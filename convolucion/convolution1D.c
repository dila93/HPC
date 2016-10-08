
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



//__global__ void matrixMultKernelTiled(float *d_matrizB, float *d_matrizA, float *d_matrizR) {   
//}


//__global__ void matrixMultKernel(float *d_matrizA, float *d_matrizB, float *d_matrizR){    
//}

void matrixMult_CPU_1D(int *h_matrizA1D, int *h_matrizB1D, int * h_matrizA1DC) {
	// M -> tamaño del vector A, y tamaño de la matriz convolusionada
  	// N -> tamaño de la mascara
	for(int i = 0; i<M; i++){
		int suma = 0;
		int N_start_point = i - (N/2);
		
		if(N_start_point >= 0){
			N_start_point = ceil(i - (N/2));
		}
	
		for(int j = 0; j<N; j++){
			if(N_start_point+j >= 0 && N_start_point+j < M){
				suma += h_matrizA1D[N_start_point+j]*h_matrizB1D[j];
			}else{
				suma += 0*h_matrizB1D[j];
			}
		}
		h_matrizA1DC[i] = suma;
	}
}


__global__ void matrixMult_GPU_1D(int *d_matrizA1D, int *d_matrizB1D, int *d_matrizA1DC) {
	int pos = blockIdx.x * blockDim.x + threadIdx.x;
	int suma = 0;
	int N_start_point = pos - (N / 2);

	for (int j = 0; j < N; j++) {
		if (N_start_point + j >= 0 && N_start_point + j < M) {
			suma += d_matrizA1D[N_start_point + j] * d_matrizB1D[j];
		}
	}
	d_matrizA1DC[pos] = suma;
}



void printCPU_1D(double time_CPU1D, int *h_matrizB1D, int *h_matrizA1D, int *h_matrizA1DC){
	//1D CPU---------------------------------------------------------------------------------------

	
	//time_GPU=(((double) (endGPU - startGPU))) / CLOCKS_PER_SEC;
	printf("---------------------------------------------------------------------------------------------\n");
	printf("Tiempo ejecuccion 1D CPU: %.10f\n",time_CPU1D);
	printf("Analisis 1D en CPU:\n\n");
	//printf("Tiempo ejecuccion GPU: %.10f\n",time_GPU);
	
	//printf("La aceleración obtenida es de %.10fX\n",time_CPU1D/time_GPU);

 
   //imprime resultado
   	printf("      Matriz B(Mask)		          Matriz A 		  MatrizA Convolusionada\n");
 	for (int i = 0; i < M; i++) {
 		if(i < N){
 			printf("	matrizB[%i]:%i 		 	matrizA[%i]: %i 		 	 matrizAC[%i]: %i \n",i,h_matrizB1D[i],i,h_matrizA1D[i],i,h_matrizA1DC[i]);
 		}else{
 			printf("					 	matrizA[%i]: %i 		 	 matrizAC[%i]: %i \n",i,h_matrizA1D[i],i,h_matrizA1DC[i]);
 		}
  	   	
	}
	//printf("--------------------------------------------------------------------------------------\n");
}


void printGPU_1D(double time_GPU1D, int *d_matrizB1D, int *d_matrizA1D, int *d_matrizA1DC){
	//1D GPU---------------------------------------------------------------------------------------

	
	//time_GPU=(((double) (endGPU - startGPU))) / CLOCKS_PER_SEC;
	printf("---------------------------------------------------------------------------------------------\n");
	printf("Tiempo ejecuccion 1D GPU: %.10f\n",time_GPU1D);
	printf("Analisis 1D en GPU:\n\n");
	//printf("Tiempo ejecuccion GPU: %.10f\n",time_GPU);
	
	//printf("La aceleración obtenida es de %.10fX\n",time_CPU1D/time_GPU);

 
   //imprime resultado
   	printf("      Matriz B(Mask)		          Matriz A 		  MatrizA Convolusionada\n");
 	for (int i = 0; i < M; i++) {
 		if(i < N){
 			printf("	matrizB[%i]:%i 		 	matrizA[%i]: %i 		 	 matrizAC[%i]: %i \n",i,d_matrizB1D[i],i,d_matrizA1D[i],i,d_matrizA1DC[i]);
 		}else{
 			printf("					 	matrizA[%i]: %i 		 	 matrizAC[%i]: %i \n",i,d_matrizA1D[i],i,d_matrizA1DC[i]);
 		}
  	   	
	}
	printf("--------------------------------------------------------------------------------------\n");}


int main(){
  
  	int *h_matrizA1D, *h_matrizB1D, *h_matrizA1DC;
  	int *d_matrizA1D, *d_matrizB1D, *d_matrizA1DC;
  	//Variables para tomar el tiempo de ejecuccion
  	clock_t start_CPU1D, endCPU1D;
	clock_t   start_GPU1D, endGPU1D;  
  	double time_CPU1D;
	double time_GPU1D;

  	int size1 = M * sizeof(int);
  	int size2 = N * sizeof(int);
  	int size3 = M * sizeof(int);

  	//Memoria para la matriz1 CPU
 	h_matrizA1D=(int*)malloc(size1);
 	//Memoria para la matriz2 CPU
 	h_matrizB1D=(int*)malloc(size2);
 	//Memoria para la matriz3 resultado CPU
 	h_matrizA1DC=(int*)malloc(size3);

  	//1D---------------------------------------
  	int cont=1;
  	//inicializacion de la matriza1 CPU
	for (int j = 0; j < M; j++) {		
		h_matrizA1D[j]=rand()%4;
		cont++;
	}		
	cont=1;
	//inicializacion de la matriz2 CPU
	for (int j = 0; j < N; j++) {
		h_matrizB1D[j]=rand()%4;
		cont++;	
	}

	//1D---------------------------------------
	//Empieza ejecuccion en la CPU
	start_CPU1D = clock();
	
	matrixMult_CPU_1D(h_matrizA1D,h_matrizB1D,h_matrizA1DC);
	
	endCPU1D = clock();

	time_CPU1D = (((double) (endCPU1D - start_CPU1D))) / CLOCKS_PER_SEC;

	printCPU_1D(time_CPU1D, h_matrizB1D, h_matrizA1D, h_matrizA1DC);
	
	//inicio algoritmo GPU 1D ---------------------------------------------------------------------
	start_GPU1D = clock();
	
	//Asignacion memoria variables para GPU
	cudaMalloc((void **)&d_matrizA1D,size1);
	cudaMalloc((void **)&d_matrizB1D,size2);
	cudaMalloc((void **)&d_matrizA1DC,size3);
	
	//copy data to divice
	cudaMemcpy(d_matrizA1D,h_matrizA1D,size1,cudaMemcpyHostToDevice);	
	cudaMemcpy(d_matrizB1D,h_matrizB1D,size2,cudaMemcpyHostToDevice);
	
	//definir las dimensiones para el kernel
	int block= 4;
	dim3 dimBlock(block, 1,1);//aCA se defiene el numero de hilos dentro de cada block
	dim3 dimGrid(ceil(M/float(block)),1,1);//aca se define el numero de block en mi grilla
  
	matrixMult_GPU_1D<<<dimGrid,dimBlock>>>(d_matrizA1D,d_matrizB1D,d_matrizA1DC);
  
	//se copian los datos del Device to CPU
	cudaMemcpy(h_matrizA1DC, d_matrizA1DC, size3,cudaMemcpyDeviceToHost);
	
	endGPU1D = clock();

	time_GPU1D = (((double) (endGPU1D - start_GPU1D))) / CLOCKS_PER_SEC;
	printGPU_1D(time_GPU1D, h_matrizB1D, h_matrizA1D, h_matrizA1DC);
  
  	free(h_matrizA1D);
  	free(h_matrizB1D);
  	free(h_matrizA1DC);

  	cudaFree(d_matrizA1D);
  	cudaFree(d_matrizB1D);
  	cudaFree(d_matrizA1DC);

  	return 0; 
}
