#include <cuda.h>
#include <stdio.h>
#define SIZE 10

int main(int argc,char *argv[]){

  if(argc<3){
    printf("Usage: ./test.cu <ptx_file> <cuda_device>\n");
    exit(0);
  }
  
  // Error code
  CUresult error;

  int i;

  // Host variables
  float *h_A, *h_B, *h_C;
  h_A = (float *)malloc(sizeof(float)*SIZE);
  h_B = (float *)malloc(sizeof(float)*SIZE);
  h_C = (float *)malloc(sizeof(float)*SIZE);
  for (i=0;i<SIZE;i++){
    h_A[i] = 12;
    h_B[i] = i;
    h_C[i] = 0;
  }

  // Initialize driver API
  error = cuInit(0);
  if((int)error!=0){
    printf("Error! cuInit returned: %d\n",(int)error); 
    exit(0);
  }

  // Get Cuda Device and give handle
  CUdevice cu_device;
  error = cuDeviceGet(&cu_device,atoi(argv[2]));
  if((int)error!=0){
    printf("Error! cuDeviceGet returned: %d\n",(int)error);
    exit(0);
  }

  // Create context to run on device 
  CUcontext cu_context;
  error = cuCtxCreate(&cu_context, 0, cu_device);
  if((int)error!=0){
    printf("Error! cuCtxCreate returned: %d\n",(int)error);
    exit(0);
  }

  // Load ptx code
  CUmodule cu_module;
  error = cuModuleLoad(&cu_module,argv[1]);
  if((int)error!=0){
    printf("Error! cuModuleLoad returned: %d\n",(int)error);
    exit(0);
  }

  // Get kernel function
  CUfunction func;
  error = cuModuleGetFunction(&func,cu_module,"VectorAdd"); 
  if((int)error!=0){
    printf("Error! cuModuleGetFunction returned: %d\n",(int)error);
    exit(0);
  }

  CUdeviceptr var1,var2,var3;

  // Allocate device memory
  unsigned int size = sizeof(float)*SIZE;

  error = cuMemAlloc(&var1, size);
  error = cuMemAlloc(&var2, size);
  error = cuMemAlloc(&var3, size);
  if((int)error!=0){
    printf("Error! cuMemAlloc returned: %d\n",(int)error);
    exit(0);
  }

  // Copy variables to host
  error = cuMemcpyHtoD(var1,h_C,size);
  error = cuMemcpyHtoD(var2,h_A,size);
  error = cuMemcpyHtoD(var3,h_B,size);
  if((int)error!=0){
    printf("Error! cuMemcpyHtoD returned: %d\n",(int)error);
    exit(0);
  }

  // Lauch kernel
  void *args[] = {&var1, &var2, &var3};
  error = cuLaunchKernel(func, 10, 1, 1, 1, 1, 1, 0, NULL, args, NULL);
  if((int)error!=0){
    printf("Error! cuLaunchKernel returned: %d\n",(int)error);
    exit(0);
  }

  // Get result to host
  error = cuMemcpyDtoH(h_C,var1,size);
  if((int)error!=0){
    printf("Error! cuMemcpyDtoH returned: %d\n",(int)error);
    exit(0);
  }

  // Free device memory
  error = cuMemFree(var1);
  error = cuMemFree(var2);
  error = cuMemFree(var3);
  if((int)error!=0){
    printf("Error! cuMemFree returned: %d\n",(int)error);
    exit(0);
  }

  // Destroy context
  error = cuCtxDestroy(cu_context);
  if((int)error!=0){
    printf("Error! cuCtxDestroy returned: %d\n",(int)error);
    exit(0);
  }


  // Print result
  for (i=0;i<SIZE;i++)
    printf("%f\n",h_C[i]);
}
