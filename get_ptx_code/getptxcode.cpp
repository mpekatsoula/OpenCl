#include <oclUtils.h>

/* Gets a cl_program and generates the PTX code */
void Getptxcode(cl_program cpProgram)  {

  FILE *file;
  file = fopen("code.ptx","a+");
  size_t binary_sizes;
  char *binaries;
  clGetProgramInfo(cpProgram, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &binary_sizes, NULL);
  binaries = (char *)malloc(sizeof(char)*binary_sizes);
  clGetProgramInfo(cpProgram, CL_PROGRAM_BINARIES, sizeof(size_t), &binaries, NULL);
  
  // Write the ptx code to file
  for (size_t j=0; j<binary_sizes; j++)
    fprintf(file,"%c", binaries[j]);
  fclose(file);


}
