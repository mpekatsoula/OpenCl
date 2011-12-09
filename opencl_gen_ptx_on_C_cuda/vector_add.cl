__kernel void VectorAdd(__global float* c, __global float* a,__global float* b){

    
   // Index of the elements to add \n
   unsigned int n = get_global_id(0);

  if (n>1024)
    return;
  
   // Sum the n’th element of vectors a and b and store in c \n
   c[n] = a[n]+b[n];
}

