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
char* cSourceCL = NULL;         // Buffer to hold source for compilation
const char* cExecutableName = NULL;

// Clean up function
void Cleanup (int argc, char **argv);

int main (int argc, char *argv[]) {

  shrQAStart(argc, argv);

  if (argc<3){
    printf("Usage: ./skel <source.cl> <kernel> <localworksize>\n");
    exit(0);

  // Load the name of the file
  cSourceFile = argv[1];

  szLocalWorkSize = atoi(argv[3]);
  szGlobalWorkSize = shrRoundUp((int)szLocalWorkSize, iNumElements);  // rounded up to the nearest multiple of the LocalWorkSize

  // Get an OpenCL platform
  ciErr1 = clGetPlatformIDs(1, &cpPlatform, NULL);
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clGetPlatformIDs.\n");
    Cleanup(argc, argv);
  }

  // Get the devices
  ciErr1 = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &cdDevice, NULL);
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clGetDeviceIDs.\n");
    Cleanup(argc, argv);
  }

  // Create the context
  cxGPUContext = clCreateContext(0, 1, &cdDevice, NULL, NULL, &ciErr1);
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clCreateContext.\n");
    Cleanup(argc, argv);
  }

  // Create a command-queue
  cqCommandQueue = clCreateCommandQueue(cxGPUContext, cdDevice, 0, &ciErr1);
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clCreateCommandQueue.\n");
    Cleanup(argc, argv);
  }

  // Allocate the OpenCL buffer memory objects for source and result on the device GMEM
  // Allocate host memory
  /************************************************************************************/


  /************************************************************************************/

  // Read the OpenCL kernel in from source file
  cPathAndName = shrFindFilePath(cSourceFile, argv[0]);
  cSourceCL = oclLoadProgSource(cPathAndName, "", &szKernelLength);

  // Create the program
  cpProgram = clCreateProgramWithSource(cxGPUContext, 1, (const char **)&cSourceCL, &szKernelLength, &ciErr1);
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clCreateProgramWithSource.\n");
    Cleanup(argc, argv);
  }

  // Build the program with 'mad' Optimization option
  char* flags = "-cl-fast-relaxed-math";
  ciErr1 = clBuildProgram(cpProgram, 0, NULL, NULL, NULL, NULL);
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clBuildProgram.\n");
    Cleanup(argc, argv);
  }

  // Create the kernel
  ckKernel = clCreateKernel(cpProgram, "VectorAdd", &ciErr1);
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clCreateKernel.\n");
    Cleanup(argc, argv);
  }

  // Set the Argument values and write data to GPU
  /************************************************************************************/
 
  
  /************************************************************************************/



  // Launch kernel
  ciErr1 = clEnqueueNDRangeKernel(cqCommandQueue, ckKernel, 1, NULL, &szGlobalWorkSize, &szLocalWorkSize, 0, NULL, NULL);
  if (ciErr1 != CL_SUCCESS)
  {
    printf("Error in clEnqueueNDRangeKernel.\n");
    Cleanup(argc, argv);
  }

  // Copy results from GPU
  /************************************************************************************/
    
  /************************************************************************************/


  // Cleanup and leave
  Cleanup (argc, argv);
}





void Cleanup (int argc, char **argv)
{
  // Cleanup allocated objects
  if(cPathAndName)free(cPathAndName);
  if(cSourceCL)free(cSourceCL);
  if(ckKernel)clReleaseKernel(ckKernel);  
  if(cpProgram)clReleaseProgram(cpProgram);
  if(cqCommandQueue)clReleaseCommandQueue(cqCommandQueue);
  if(cxGPUContext)clReleaseContext(cxGPUContext);


  // Free memory on host and GPU
  /************************************************************************************/
    
  /************************************************************************************/
  exit(0);
}

