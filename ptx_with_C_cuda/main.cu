#include <cuda.h>
#include <stdio.h>

int main(int argc,char *argv[]){

  if(argc<3){
    printf("Usage: ./test.cu <ptx_file> <cuda_device>\n");
    exit(0);
  }
  
  // Error code
  CUresult error;
  
  // My number
  unsigned int h_var=7;
  
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
  error = cuModuleGetFunction(&func,cu_module,"testing"); 
  if((int)error!=0){
    printf("Error! cuModuleGetFunction returned: %d\n",(int)error);
    exit(0);
  }

  CUdeviceptr var;

  // Allocate device memory
  unsigned int size = sizeof(unsigned int);
  error = cuMemAlloc(&var, size);
  if((int)error!=0){
    printf("Error! cuMemAlloc returned: %d\n",(int)error);
    exit(0);
  }

  // Copy variable to host
  error = cuMemcpyHtoD(var,&h_var,size);
  if((int)error!=0){
    printf("Error! cuMemcpyHtoD returned: %d\n",(int)error);
    exit(0);
  }

  // Lauch kernel
  void *args[] = {&var};
  error = cuLaunchKernel(func, 1, 1, 1, 1, 1, 1, 0, NULL, args, NULL);
  if((int)error!=0){
    printf("Error! cuLaunchKernel returned: %d\n",(int)error);
    exit(0);
  }

  // Get result to host
  error = cuMemcpyDtoH(&h_var,var,size);
  if((int)error!=0){
    printf("Error! cuMemcpyDtoH returned: %d\n",(int)error);
    exit(0);
  }

  // Free device memory
  error = cuMemFree(var);
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
  printf("var: %d\n",h_var);
}
