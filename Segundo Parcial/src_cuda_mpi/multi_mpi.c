/******************************************************************************
* FILE: mpi_sumBig.c
* DESCRIPTION:  
*   Suma de dos vectores de gran tamaño 10^6=10.000.000
*   
*   
*   
*   
*   
* AUTHOR:Douglas Hernandez 
*   
* Realizado:26/10/2016
******************************************************************************/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "extern.h"

#define NRA 5000         // number of rows in matrix A
#define NCA 4000         // number of columns in matrix A
#define NCB 5000         // number of columns in matrix B
#define MASTER 0      // taskid of first task
#define FROM_MASTER 1 // setting a message type
#define FROM_WORKER 2 // setting a message type


void mult_mat_CUDA(double *h_a, double *h_b, double *h_c, int height,int width_a, int width_b);

void init_buf(double *init_buf, int y, int x);
//void MPI_Multiply(double *a, double *b, double *c, int rows, int y, int x);
bool compare_resultado(double *mat_MPI, double *mat_CUDA, int y, int x);

void MPI_Multiply(double *matriz_a, double *matriz_b, double *matriz_r, int rows, int y, int x) {
  


  double suma;
  for (int j = 0; j < x; j++) {
    for (int i = 0; i < rows; i++) {
      for (int k = 0; k < y; k++) {
        //matriz_r[i * x + j] += matriz_a[i * y + k] * matriz_b[k * x + j];
        suma +=matriz_a[i * y + k] * matriz_b[k * x + j];
      }
      matriz_r[i * x + j]=suma;
    }
  }
}

void print(double *mat, int h, int w) {
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      printf("%f \n",mat[i * w + j] );
      //cout << mat[i * w + j] << "  ";
    }
    //cout << endl;
  }
}

int main (int argc, char *argv[]){

clock_t start_CPU, endCPU, startGPU, endGPU;//Variables para tomar el tiempo de ejecuccion
double time_CPU, time_GPU;

int   numtask,              /* number of tasks in partition */
   taskid,                /* a task identifier */
   numworkers,            /* number of worker tasks */
   source,                /* task id of message source */
   dest,                  /* task id of message destination */
   mtype,                 /* message type */
   cols,
   rows,                  /* rows of matrix A sent to each worker */
   averow, extra, offset, /* used to determine rows sent to each worker */
   i, j, k, rc;           /* misc */
//float  //a[N],           /* vector 1 */
   //b[N],           /* vector 2 */
   //c[N];         /* resultado de la suma de 1 y 2 */

/*float* a=(float*)malloc(sizeof(float)*N); 
float* b=(float*)malloc(sizeof(float)*N); 
float* c=(float*)malloc(sizeof(float)*N);*/ 
   
MPI_Status status;

MPI_Init(NULL,NULL);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
MPI_Comm_size(MPI_COMM_WORLD,&numtask);
if (numtask < 2 ) {
  printf("Need at least two MPI tasks. Quitting...\n");
  MPI_Abort(MPI_COMM_WORLD, rc);
  exit(1);
  }
numworkers = numtask-1;


/**************************** master task ************************************/
   if (taskid == MASTER){

      double *a = new double[NRA * NCA];      // matrix A to be multiĺied
      double *b = new double[NCA * NCB];      // matrix B to be multiplied
      double *c_MPI = new double[NRA * NCB];  // reesult matrix C
      double *c_CUDA = new double[NRA * NCB]; // reesult matrix C with CUDA
     //printf("mpi_mm has started with %d tasks.\n",numtasks);
      printf("Initializing arrays...\n");
      
      init_buf(a, NRA, NCA);
      init_buf(b, NCA, NCB);
   
   
      /* Send matrix data to the worker tasks */
      averow = NRA/numworkers;
      extra = NRA%numworkers;
      offset = 0;
      mtype = FROM_MASTER;

      startGPU = clock();

      for (dest=1; dest<=numworkers; dest++)
      {
         rows = (dest <= extra) ? averow+1 : averow;     
         //printf("Sending %d cols to task %d offset=%d\n",cols,dest,offset);
         MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
         MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
         MPI_Send(&a[offset * NCA], rows * NCA, MPI_DOUBLE, dest, mtype,MPI_COMM_WORLD);
         MPI_Send(b, NCA * NCB, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
         offset = offset + rows;
      }

      /* Receive results from worker tasks */
      mtype = FROM_WORKER;
      for (i=1; i<=numworkers; i++)
      {
         source = i;
         MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
         MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
         MPI_Recv(&c_MPI[offset * NCB], rows * NCB, MPI_DOUBLE, source, mtype,MPI_COMM_WORLD, &status);
         MPI_Recv(&c_CUDA[offset * NCB], rows * NCB, MPI_DOUBLE, source, mtype,MPI_COMM_WORLD, &status);
         printf("Received results from task %d\n",source);
      }

      endGPU = clock();

      //time_CPU = (((double) (endCPU - start_CPU))) / CLOCKS_PER_SEC;
      time_GPU=(((double) (endGPU - startGPU))) / CLOCKS_PER_SEC;
      //printf("Tiempo ejecuccion CPU: %.10f\n",time_CPU);
      printf("Tiempo ejecuccion GPU: %.10f\n",time_GPU);
      //print(c_MPI,NCA,NCB);
      //printf("La aceleración obtenida es de %.10fX\n",time_CPU/time_GPU);

      /*if (compare_resultado(c_MPI, c_CUDA, NRA, NCB)) {
         printf("Buen calculo!");
      } else {
         printf("Mal calculo!");
      }*/
   }


/**************************** worker task ************************************/
   if (taskid > MASTER){

      mtype = FROM_MASTER;
      MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);

      double *a = new double[rows * NCA];
      double *b = new double[NCA * NCB];
      double *c_MPI = new double[rows * NCB];
      double *c_CUDA = new double[rows * NCB];

      MPI_Recv(a, rows * NCA, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD,&status);
      MPI_Recv(b, NCA * NCB, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD,&status);

      //ak llamaria la funcion de cuda y cpu

      // version CPU
      //start_CPU = clock();
      //MPI_Multiply(a, b, c_MPI, rows, NCA, NCB);
      //endCPU = clock();
      // Version CUDA
      //startGPU = clock();
      mult_mat_CUDA(a, b, c_CUDA, rows, NCA, NCB);
      //endGPU = clock();
         
      mtype = FROM_WORKER;
      MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
      
      MPI_Send(c_MPI, rows * NCB, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);
      MPI_Send(c_CUDA, rows * NCB, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);
      
      free(a);
      free(b);
      free(c_MPI);
      free(c_CUDA);
   }
   MPI_Finalize();
   
}
