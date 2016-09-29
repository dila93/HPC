#include <stdio.h>
#include <cuda.h>
#include <stdlib.h>
#include<time.h>

#define M 3000
#define N 500
#define P 200

#define TILE_WIDTH 4


__global__ void matrixMultKernelTiled(float *d_matrizB, float *d_matrizA, float *d_matrizR) {
  
  //declaracion del tamaño de la memoria compartida
  __shared__ float ds_M[TILE_WIDTH][TILE_WIDTH];
  __shared__ float ds_N[TILE_WIDTH][TILE_WIDTH];

  //identificadores de los hilos y bloques
  int bx = blockIdx.x;
  int by = blockIdx.y;
  int tx = threadIdx.x;
  int ty = threadIdx.y;

  //dimension de las matriz a operar
  int row = by * TILE_WIDTH + ty;
  int col = bx * TILE_WIDTH + tx;

  //variable para almacenar el resultado de la multiplicacion
  float suma = 0;

  //se recorre el numero de fases necesarias para obtener los resultados
  for (int p = 0; p < (TILE_WIDTH + N-1)/TILE_WIDTH; p++) {
	 
	//se asigna los valores de las filas a la memoria compartida
	//en la matriz ds_M
    if (row < M && (p * TILE_WIDTH + tx) < N) {
      ds_M[ty][tx] = d_matrizA[(row * N) + (p * TILE_WIDTH) + tx];
    } else {
      ds_M[ty][tx] = 0.0;
    }

	//se asigna los valores de las columnas a la memoria compartida
	//en la matriz ds_N
    if ((p * TILE_WIDTH + ty) < N and col < P) {
      ds_N[ty][tx] = d_matrizB[(((p * TILE_WIDTH) + ty) * P) + col];
    } else {
      ds_N[ty][tx] = 0.0;
    }
	//sincronizacion de los hilos, esperar hasta que todos esten ak
    __syncthreads();

    if (row < M and col < P){
		for (int k = 0; k < TILE_WIDTH; k++) {
        suma += ds_M[ty][k] * ds_N[k][tx];
      }
    __syncthreads();
	}
      
  }

  if (row < M and col < P){
	d_matrizR[row * P + col] = suma;  
  }
    
}


__global__ void matrixMultKernel(float *d_matrizA, float *d_matrizB, float *d_matrizR){
    int row = blockIdx.y*blockDim.y+threadIdx.y;
    int col = blockIdx.x*blockDim.x+threadIdx.x;
  	
    float sum;
    if((row < M)&&(col < P)){
      //printf("Valores row col: %i---%i\n",row,col);
        sum = 0;
        for (int k = 0; k < N ; ++k){
            sum += d_matrizA[row*N+k] * d_matrizB[k*P+col];
        } 
      d_matrizR[((row)*(P))+col] = sum;
    }
   
}

void matrixMultCPU2(float *h_matrizA, float *h_matrizB, float * h_matrizR) {
 float sum;
  
	for(int row = 0; row < M ; row++){
        for(int col = 0; col < P ; col++){
            sum = 0;
            for(int k = 0; k < N ; ++k){
                sum += h_matrizA[row*N+k] * h_matrizB[k*P+col];
              //printf("suma: %f\n",sum);
            }
            h_matrizR[(row*(P))+col] = sum;
        }
    } 
  //imprime resultado
 /*for (int i = 0; i < M*P; i++) {
  //for (int j = 0; j < P; j++) {
     printf("Resultado de la multiplicación CPU: %i---%f\n",i,h_matrizR[i]);
  //}
 }*/
 
}

int testValues(float *A, float *B, float width){

    for(int i = 0; i < P*M; ++i){
       // for(int j = 0; j < M; ++j){
            if(A[i]!=B[i]){
                printf("Mal Cálculo...\n");
              	printf("valor GPU: %i---%f\n",i,A[i]);
              	printf("valor CPU: %i---%f\n",i,B[i]);
                return 0;
            }
        //}
    }
    printf("Buen Cálculo ...\n");
    return 0;
}


int main(){
  
  float *h_matrizA, *h_matrizB, *h_matrizR,*h_matrizRGPU ;
  
  clock_t start_CPU, endCPU, startGPU, endGPU;//Variables para tomar el tiempo de ejecuccion
  double time_CPU, time_GPU;
  
  float *d_matrizA, *d_matrizB,*d_matrizR;//variables matrices divice
  
  float size1 = M * N * sizeof(float);
  float size2 = N * P * sizeof(float);
  float size3 = M * P * sizeof(float);
  //Memoria para la matriz1 CPU
  h_matrizA= (float*)malloc(size1);
  //Memoria para la matriz2 CPU
  h_matrizB=(float*)malloc(size2);
  //Memoria para la matriz3 resultado CPU
  h_matrizR=(float*)malloc(size3);
  h_matrizRGPU=(float*)malloc(size3);
  
  float cont=1;
  //inicializacion de las matriz1 CPU
	for (int j = 0; j < M; j++) {		
		for(int i=0; i<N; i++){
			h_matrizA[j*N+i]=rand()%3;
			cont++;
		}		
	}
	cont=1;
	//inicializacion de las matriz2 CPU
	for (int j = 0; j < N; j++) {		
		for(int i=0; i<P; i++){
			h_matrizB[j*P+i]=rand()%3;
			cont++;
		}		
	}
	
	//Empieza ejecuccion en la CPU
	start_CPU = clock();
	
	matrixMultCPU2(h_matrizA,h_matrizB,h_matrizR);
	
	endCPU = clock();
	/*for (int j = 0; j < M*N; j++) {
		printf("valores de la mA: %i---%f\n",j,h_matrizA[j]);		
	}
	
	for (int j = 0; j < N*P; j++) {
		printf("valores de la mB: %i---%f\n",j,h_matrizB[j]);		
	}*/
	
	
	//****inicio algoritmo GPU*****//
	startGPU = clock();
	//Asignacion memoria variables para GPU
	cudaMalloc((void **)&d_matrizA,size1);
	cudaMalloc((void **)&d_matrizB,size2);
	cudaMalloc((void **)&d_matrizR,size3);
	
	//copy data to divice
	cudaMemcpy(d_matrizA,h_matrizA,size1,cudaMemcpyHostToDevice);
	cudaMemcpy(d_matrizB,h_matrizB,size2,cudaMemcpyHostToDevice);
	
	//definir las dimensiones para el kernel
	int block= 32;
	dim3 dimBlock(block, block,1);//ak se defiene el numero de hilos dentro de cada block
	dim3 dimGrid(ceil(P/float(block)),ceil(M/float(block)),1);//ak se define el numero de block en mi grilla
	
	//se lanza el Kernel-- se puede lanzar el kernel multiplicacion normal * con tiled
	//solamente se debe comentar una linea y descomentar la otra para que no
	//sea pesado para el servidor
  
	matrixMultKernel<<<dimGrid,dimBlock>>>(d_matrizA,d_matrizB,d_matrizR);
	//matrixMultKernelTiled<<<dimGrid,dimBlock>>>(d_matrizA,d_matrizB,d_matrizR);
  
	//se copiand los datos del Device to CPU
	cudaMemcpy(h_matrizRGPU, d_matrizR, size3,cudaMemcpyDeviceToHost);
	
	endGPU = clock();
  
  //Se imprimen los tiempos de ejecuccion 
	time_CPU = (((double) (endCPU - start_CPU))) / CLOCKS_PER_SEC;
	time_GPU=(((double) (endGPU - startGPU))) / CLOCKS_PER_SEC;
	printf("Tiempo ejecuccion CPU: %.10f\n",time_CPU);
	printf("Tiempo ejecuccion GPU: %.10f\n",time_GPU);
	
	printf("La aceleración obtenida es de %.10fX\n",time_CPU/time_GPU);

 
   //imprime resultado
 /* for (int i = 0; i < M*P; i++) {
     printf("Resultado de la multiplicación GPU: %i---%f\n",i,h_matrizRGPU[i]);
  }*/
  
  testValues(h_matrizRGPU,h_matrizR,N);
  
  free(h_matrizA);
  free(h_matrizB);
  free(h_matrizR);
  cudaFree(d_matrizA);
  cudaFree(d_matrizB);
  cudaFree(d_matrizR);
  return 0; 
}