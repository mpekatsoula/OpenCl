/* Scans the machine and fetches info for the platform and the devices */


#include <CL/cl.h>
#include <stdio.h>

int main(void) {


  /***************** Platforms *****************/
  /* How many platforms we want */
  const cl_uint max_num_platforms = 10;
  /* Array that holds the platforms ids */
  cl_platform_id platforms_ids[max_num_platforms];
  /* Number of platforms found */
  cl_uint num_platforms;

  /* Get platforms */
  (void)clGetPlatformIDs(max_num_platforms, platforms_ids, &num_platforms);
  printf("Found %d platforms\n\n", num_platforms);

  /***************** Devices *****************/
  /* How many devices we want */
  const cl_uint max_num_devs = 10;
  /* Array that holds the devices ids */
  cl_device_id device_ids[max_num_devs];
  /* Number of devices found */
  cl_uint num_devices;

  char str_result[1024];
  size_t len,param_value_size_ret;
  cl_uint u_info;
  cl_bool bool_info;
  cl_device_type param_value;
  /* Loop through avaliable platforms to scan for devices */
  unsigned int i,k;
  for(i = 0; i < num_platforms; i++) {
    printf(">Platform %d:\n",i);
    
    /* CL_PLATFORM_PROFILE   : Returns the profile name supported by the implementation. 
       CL_PLATFORM_VERSION   : Returns the OpenCL version supported by the implementation.
       CL_PLATFORM_NAME      : Platform name string.
       CL_PLATFORM_VENDOR    : Platform vendor string.
       CL_PLATFORM_EXTENSIONS:
    */
    clGetPlatformInfo ( platforms_ids[i], CL_PLATFORM_PROFILE, sizeof(str_result), str_result, &len );
    printf("\tCL_PLATFORM_PROFILE   : %s\n",str_result);
    clGetPlatformInfo ( platforms_ids[i], CL_PLATFORM_VERSION, sizeof(str_result), str_result, &len );
    printf("\tCL_PLATFORM_VERSION   : %s\n",str_result);
    clGetPlatformInfo ( platforms_ids[i], CL_PLATFORM_NAME, sizeof(str_result), str_result, &len );
    printf("\tCL_PLATFORM_NAME      : %s\n",str_result);
    clGetPlatformInfo ( platforms_ids[i], CL_PLATFORM_VENDOR, sizeof(str_result), str_result, &len );
    printf("\tCL_PLATFORM_VENDOR    : %s\n",str_result);
    clGetPlatformInfo ( platforms_ids[i], CL_PLATFORM_EXTENSIONS, sizeof(str_result), str_result, &len );
    printf("\tCL_PLATFORM_EXTENSIONS: %s\n",str_result);

    /* Get the devices from the current platform 
       CL_DEVICE_TYPE_ALL: All OpenCL devices available in the system. */
    clGetDeviceIDs(platforms_ids[i], CL_DEVICE_TYPE_ALL, max_num_devs, device_ids, &num_devices);
    printf("\n\tFound %d devices\n", num_devices);

    /* Loop through devices to fetch info */
    for (k = 0; k < num_devices; k++) {
      printf("\t\t~Device %d:\n",k);

      /* Get and print type */
      clGetDeviceInfo (device_ids[k], CL_DEVICE_TYPE, sizeof(param_value), &param_value, &param_value_size_ret);
      if (param_value & CL_DEVICE_TYPE_CPU)
        printf("\t\t\tType: CPU\n");
      if (param_value & CL_DEVICE_TYPE_GPU)
        printf("\t\t\tType: GPU\n");
      if (param_value & CL_DEVICE_TYPE_ACCELERATOR)
        printf("\t\t\tType: ACCELERATOR\n");
      if (param_value & CL_DEVICE_TYPE_DEFAULT)
        printf("\t\t\tType: DEFAULT\n");

      /*  CL_DEVICE_PROFILE                 : Returns the profile name supported by the device.
          CL_DEVICE_VERSION                 : Returns the OpenCL version supported by the device.
          CL_DEVICE_NAME                    : Device name string.
          CL_DEVICE_VENDOR                  : Vendor name string.
          CL_DEVICE_EXTENSIONS              : Returns a space separated list of extension names.
          CL_DEVICE_VENDOR_ID               : A unique device vendor identifier.
          CL_DEVICE_MAX_COMPUTE_UNITS       : The number of parallel compute cores on the OpenCL device.
          CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: Maximum dimensions that specify the global and local work-item IDs used by the data parallel execution model.
          CL_DEVICE_MAX_WORK_ITEM_SIZES     : Maximum number of work-items that can be specified in each dimension of the work-group to clEnqueueNDRangeKernel.
          CL_DEVICE_AVAILABLE               : Is CL_TRUE if the device is available and CL_FALSE if the device is not available.
      */
      clGetDeviceInfo (device_ids[k], CL_DEVICE_PROFILE, sizeof(str_result), str_result, &len );
      printf("\t\t\tCL_DEVICE_PROFILE                 : %s\n", str_result);
      clGetDeviceInfo (device_ids[k], CL_DEVICE_VERSION, sizeof(str_result), str_result, &len );
      printf("\t\t\tCL_DEVICE_VERSION                 : %s\n", str_result);
      clGetDeviceInfo (device_ids[k], CL_DEVICE_NAME, sizeof(str_result), str_result, &len );
      printf("\t\t\tCL_DEVICE_NAME                    : %s\n", str_result);
      clGetDeviceInfo (device_ids[k], CL_DEVICE_VENDOR, sizeof(str_result), str_result, &len );
      printf("\t\t\tCL_DEVICE_VENDOR                  : %s\n", str_result);
      clGetDeviceInfo (device_ids[k], CL_DEVICE_EXTENSIONS, sizeof(str_result), str_result, &len );
      printf("\t\t\tCL_DEVICE_EXTENSIONS              : %s\n", str_result);
      clGetDeviceInfo (device_ids[k], CL_DEVICE_VENDOR_ID, sizeof(u_info), &u_info, &len );
      printf("\t\t\tCL_DEVICE_VENDOR_ID               : %u\n", u_info);
      clGetDeviceInfo (device_ids[k], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(u_info), &u_info, &len );
      printf("\t\t\tCL_DEVICE_MAX_COMPUTE_UNITS       : %u\n", u_info);
      clGetDeviceInfo (device_ids[k], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(u_info), &u_info, &len );
      printf("\t\t\tCL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %u\n", u_info);
      clGetDeviceInfo (device_ids[k], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(u_info), &u_info, &len );
      printf("\t\t\tCL_DEVICE_MAX_WORK_ITEM_SIZES     : %u\n", u_info);
      clGetDeviceInfo (device_ids[k], CL_DEVICE_AVAILABLE, sizeof(bool_info), &bool_info, &len );
      printf("\t\t\tCL_DEVICE_AVAILABLE               : %s\n\n", bool_info?"TRUE":"FALSE");


    }
  }
}
