#include <oclUtils.h>
#include <shrQATest.h>

// Name of the file with the source code for the computation kernel
const char* cSourceFile;

// OpenCL Vars
cl_context cxGPUContext;        // OpenCL context
cl_command_queue cqCommandQueue;// OpenCL command que
cl_platform_id cpPlatform;      // OpenCL platform
cl_device_id cdDevice;          // OpenCL device
cl_program cpProgram;           // OpenCL program
cl_kernel ckKernel;             // OpenCL kernel
size_t szGlobalWorkSize;        // 1D var for Total # of work items
size_t szLocalWorkSize;		      // 1D var for # of work items in the work group	
size_t szParmDataBytes;		    	// Byte size of context information
size_t szKernelLength;			    // Byte size of kernel code
cl_int ciErr1, ciErr2;		    	// Error code var
char* cPathAndName = NULL;      // var for full paths to data, src, etc.
const unsigned char* cSourceCL = NULL;         // Buffer to hold source for compilation
const char* cExecutableName = NULL;

// Device memory objects
/************************************************************************************/
cl_mem cmDevSrcA;               // OpenCL device source buffer A
cl_mem cmDevSrcB;               // OpenCL device source buffer B 
cl_mem cmDevDst;                // OpenCL device destination buffer  
/************************************************************************************/

// Clean up function
void Cleanup ();

int main (int argc, char *argv[]) {


  if (argc<3){
    printf("Usage: ./skel <source.cl> <kernel> <localworksize>\n");
    exit(0);

  }
  shrQAStart(argc, argv);

  int i,SIZE;
  SIZE = 10;

  // Load the name of the file
  cSourceFile = (const char*)argv[1];

  szLocalWorkSize = atoi(argv[3]);
  szGlobalWorkSize = shrRoundUp((int)szLocalWorkSize, SIZE);  // rounded up to the nearest multiple of the LocalWorkSize

  // Get an OpenCL platform
  ciErr1 = clGetPlatformIDs(1, &cpPlatform, NULL);
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clGetPlatformIDs.\n");
    Cleanup();
  }

  // Get the devices
  ciErr1 = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &cdDevice, NULL);
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clGetDeviceIDs.\n");
    Cleanup();
  }

  // Create the context
  cxGPUContext = clCreateContext(0, 1, &cdDevice, NULL, NULL, &ciErr1);
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clCreateContext.\n");
    Cleanup();
  }

  // Create a command-queue
  cqCommandQueue = clCreateCommandQueue(cxGPUContext, cdDevice, 0, &ciErr1);
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clCreateCommandQueue.\n");
    Cleanup();
  }

  // Allocate the OpenCL buffer memory objects for source and result on the device GMEM
  // Allocate host memory
  /************************************************************************************/
  float *h_A = (float *)malloc(sizeof(cl_float)*SIZE);
  float *h_B = (float *)malloc(sizeof(cl_float)*SIZE);
  for (i=0;i<SIZE;i++){
    h_A[i] = 12;
    h_B[i] = i;
  }
  cmDevSrcA = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, sizeof(cl_float) * szGlobalWorkSize, NULL, &ciErr1);
  cmDevSrcB = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, sizeof(cl_float) * szGlobalWorkSize, NULL, &ciErr2);
  ciErr1 |= ciErr2;
  cmDevDst = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, sizeof(cl_float) * szGlobalWorkSize, NULL, &ciErr2);
  ciErr1 |= ciErr2;
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clCreateBuffer.\n"); 
    Cleanup();
  }
  /************************************************************************************/

  // Read the PTX kernel in from source file
  //cPathAndName = shrFindFilePath(cSourceFile, argv[1]);
  cSourceCL = (const unsigned char *)oclLoadProgSource("code.ptx", "", &szKernelLength);

  // Create the program
  cpProgram = clCreateProgramWithBinary(cxGPUContext, 1, &cdDevice, &szKernelLength, &cSourceCL, NULL, &ciErr1);
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clCreateProgramWithBinary.\n");
    Cleanup();
  }

  // Build the program
  ciErr1 = clBuildProgram(cpProgram, 0, NULL, NULL, NULL, NULL);
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clBuildProgram.\n");
    Cleanup();
  }

  // Create the kernel
  ckKernel = clCreateKernel(cpProgram, argv[2], &ciErr1);
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clCreateKernel.\n");
    Cleanup();
  }

  // Set the Argument values and write data to GPU
  /************************************************************************************/
  ciErr1 = clSetKernelArg(ckKernel, 1, sizeof(cl_mem), (void*)&cmDevSrcA);
  ciErr1 |= clSetKernelArg(ckKernel, 2, sizeof(cl_mem), (void*)&cmDevSrcB);
  ciErr1 |= clSetKernelArg(ckKernel, 0, sizeof(cl_mem), (void*)&cmDevDst);
  ciErr1 = clEnqueueWriteBuffer(cqCommandQueue, cmDevSrcA, CL_FALSE, 0, sizeof(cl_float) * szGlobalWorkSize, h_A, 0, NULL, NULL);
  ciErr1 |= clEnqueueWriteBuffer(cqCommandQueue, cmDevSrcB, CL_FALSE, 0, sizeof(cl_float) * szGlobalWorkSize, h_B, 0, NULL, NULL);
  /************************************************************************************/

  
  // Launch kernel
  ciErr1 = clEnqueueNDRangeKernel(cqCommandQueue, ckKernel, 1, NULL, &szGlobalWorkSize, &szLocalWorkSize, 0, NULL, NULL);
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clEnqueueNDRangeKernel.\n");
    Cleanup();
  }

  // Copy results from GPU
  /************************************************************************************/
  ciErr1 = clEnqueueReadBuffer(cqCommandQueue, cmDevDst, CL_TRUE, 0, sizeof(cl_float) * szGlobalWorkSize, h_A, 0, NULL, NULL);
  /************************************************************************************/


  for (i=0;i<SIZE;i++){
    printf("%f\n",h_A[i]);
  }
  // Cleanup and leave
  Cleanup ();

}


void Cleanup ()
{
  // Cleanup allocated objects
  if(cPathAndName)free(cPathAndName);
  //if(cSourceCL)free(cSourceCL);
  if(ckKernel)clReleaseKernel(ckKernel);  
  if(cpProgram)clReleaseProgram(cpProgram);
  if(cqCommandQueue)clReleaseCommandQueue(cqCommandQueue);
  if(cxGPUContext)clReleaseContext(cxGPUContext);


  // Free memory on host and GPU
  /************************************************************************************/
  if(cmDevSrcA)clReleaseMemObject(cmDevSrcA);
  if(cmDevSrcB)clReleaseMemObject(cmDevSrcB);
  if(cmDevDst)clReleaseMemObject(cmDevDst);
  /************************************************************************************/
  exit(0);
}

