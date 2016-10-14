#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <cv.h>
#include <time.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

#define MASK_WIDTH 3
#define MASK_HEIGHT 3


__global__ void sobelFilterGPu(unsigned char *data_img, int *G_x, int *G_y,unsigned char *grad_x, unsigned char *grad_y,  unsigned char *grad, int y, int x) {

  int row = blockIdx.y * blockDim.y + threadIdx.y;
  int col = blockIdx.x * blockDim.x + threadIdx.x;

  if (row < y && col < x) {
    int Px_suma = 0;
    int Py_suma = 0;

    int row_start_point = row - (MASK_HEIGHT / 2);
    int col_start_point = col - (MASK_WIDTH / 2);

    for (int i = 0; i < MASK_HEIGHT; i++) {
      for (int j = 0; j < MASK_WIDTH; j++) {
        if ((row_start_point + i) >= 0 && (row_start_point + i) < y && (col_start_point + j) >= 0 && (col_start_point + j) < x) {
          
          Px_suma += (int)data_img[x * (row_start_point + i) + (col_start_point + j)] * G_x[MASK_WIDTH * i + j];
          Py_suma += (int)data_img[x * (row_start_point + i) + (col_start_point + j)] * G_y[MASK_WIDTH * i + j];
        }
      }
    }

    //grad_x[x * row + col] = Px_suma;
   // grad_y[x * row + col] = Py_suma;
    
    float grad_value = sqrtf((Px_suma * Px_suma) + (Py_suma * Py_suma));
    grad[x * row + col] = (unsigned char)(grad_value);
  }

  //esta parte del codigo es para aplicar ya como tal el filtro
  //de sobel segun la documentacion y la formula
  
   // unsigned char currGrad_x = grad_x[x * row + col];
//unsigned char currGrad_y = grad_y[x * row + col];

    
    

}

void sobelFilter(Mat &image, Mat &image_gray, Mat &grad) {
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;

  GaussianBlur(image, image, Size(3, 3), 0, 0, BORDER_DEFAULT);
  cvtColor(image, image_gray, COLOR_RGB2GRAY);

  Mat grad_x, grad_y;
  Mat abs_grad_x, abs_grad_y;

  Sobel(image_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
  convertScaleAbs(grad_x, abs_grad_x);

  Sobel(image_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
  convertScaleAbs(grad_y, abs_grad_y);

  addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
  
  //imwrite("./outputs/1088002980.png",grad);
}

int main() {
  //cudaError_t error = cudaSuccess;
  clock_t startCPU, endCPU, startGPU, endGPU;
  double cpu_time_used, gpu_time_used;
  unsigned char *d_grad_x, *d_grad_y, *h_grad, *d_grad;
  int *d_G_x, *d_G_y;
  unsigned char *d_dataRawImage, *h_dataRawImage;

  int h_mask_x[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
  int h_mask_y[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};

  Mat image, image_gray, grad;
  image = imread("./inputs/img3.jpg", CV_LOAD_IMAGE_COLOR);

  // CPU sobel
  startCPU = clock();
  sobelFilter(image, image_gray, grad);
  endCPU = clock();
  
  Size s = image_gray.size();
  int x = s.width;//se obtiene el ancho y el alto de la imagen en escala de grises
  int y = s.height;
  int size = sizeof(unsigned char) * x * y;
  int maskSize = sizeof(int) * MASK_WIDTH * MASK_HEIGHT;
  h_dataRawImage = new unsigned char[x * y];
  h_grad = new unsigned char[x * y];
  
  startGPU = clock();
  
  cudaMalloc(&d_G_x, maskSize);//memoria para los filtros en X e Y
  cudaMalloc(&d_G_y, maskSize);  
  cudaMalloc(&d_grad_x, size);//memoria para la imagen de grises en x
  cudaMalloc(&d_grad_y, size);//memoria para la imagen de grises en x
  
  cudaMalloc(&d_grad, size);//memoria para la imagen con filtro
  cudaMalloc(&d_dataRawImage, size);//tamaño de la imagen en grises
  
  h_dataRawImage = image_gray.data;
  cudaMemcpy(d_dataRawImage, h_dataRawImage, size, cudaMemcpyHostToDevice);//se copian los datos de la imagen en grises al device
  
  cudaMemcpy(d_G_x, h_mask_x, maskSize, cudaMemcpyHostToDevice);
  
  cudaMemcpy(d_G_y, h_mask_y, maskSize, cudaMemcpyHostToDevice);
  
  int blockSize = 32;
  dim3 dimBlock(blockSize, blockSize, 1);
  dim3 dimGrid(ceil(x / float(blockSize)), ceil(y / float(blockSize)), 1);
  sobelFilterGPu<<< dimGrid, dimBlock >>>(d_dataRawImage, d_G_x, d_G_y, d_grad_x, d_grad_y,d_grad, y, x);
  cudaMemcpy(h_grad, d_grad, size, cudaMemcpyDeviceToHost);
  endGPU = clock();
  
  Mat filterImg;
  filterImg.create(y, x, CV_8UC1);
  filterImg.data = h_grad;
  
  imwrite("./outputs/1088002980.png",filterImg);
  
  free(h_grad);
  cudaFree(d_dataRawImage);
  cudaFree(d_G_x);
  cudaFree(d_G_y);
  cudaFree(d_grad_x);
  cudaFree(d_grad_y);
  cudaFree(d_grad);

  return 0;
}
