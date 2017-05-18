__kernel void heat_diff( __global float* data, const unsigned int rows, const unsigned int cols, float c, unsigned int iter) {
  float h, h_l, h_r, h_u, h_d;
  //float c = 1.0f/30; //FIXME: kill me

  // We transform the indices to match the padded vector
  unsigned int transformedId = get_global_id(0) + (cols + 1) + 2 * (get_global_id(0) / (cols - 2));

  unsigned int i = transformedId / cols;
  unsigned int j = transformedId % cols;

  h = data[(iter % 2) * cols * rows + i*cols+j];
  h_l = data[(iter % 2) * cols * rows + i*cols+j-1];
  h_r = data[(iter % 2) * cols * rows + i*cols+j+1];
  h_u = data[(iter % 2) * cols * rows + (i-1)*cols+j];
  h_d = data[(iter % 2) * cols * rows + (i+1)*cols+j];

  data[((iter + 1) % 2) * cols * rows + i * cols + j] = (1-c)*h+c*(h_l+h_r+h_u+h_d)/4;
}

__kernel void sum(__global const float *input, __global float *output, __local float *reductionSums, const unsigned int cols) {
  const int globalID = get_global_id(0);
  const uint transformedId = globalID + (cols + 1) + 2 * (globalID / (cols - 2));
  const int localID = get_local_id(0);
  const int localSize = get_local_size(0);
  const int globalSize = get_global_size(0);
  const int workgroupID = globalID / localSize;

  reductionSums[localID] = input[transformedId];

  for(int offset = localSize >> 1; offset > 0; offset >>= 1) {
    barrier(CLK_LOCAL_MEM_FENCE); // wait for all other work-items to finish previous iteration.
    if(localID < offset) {
      reductionSums[localID] += reductionSums[localID + offset];
      reductionSums[localID] /= 2;
    }
  }

  if(localID == 0) {  // the root of the reduction subtree
    output[workgroupID] = reductionSums[0];
  }
  //printf("\nglobal id %d global size %d workgroupid %d local size %d local id %d transformedId %d\n", globalID,globalSize, workgroupID,localSize,localID, transformedId);
}

__kernel void std(__global const float *input, __global float *output, __local float *reductionStds, const unsigned int cols, const float average) {
  const int globalID = get_global_id(0);
  const uint transformedId = globalID + (cols + 1) + 2 * (globalID / (cols - 2));
  const int localID = get_local_id(0);
  const int localSize = get_local_size(0);
  const int globalSize = get_global_size(0);
  const int workgroupID = globalID / localSize;

  reductionStds[localID] = fabs(input[transformedId] - average);

  for(int offset = localSize >> 1; offset > 0; offset >>= 1) {
    barrier(CLK_LOCAL_MEM_FENCE); // wait for all other work-items to finish previous iteration.
    if(localID < offset) {
      reductionStds[localID] += reductionStds[localID + offset];
      reductionStds[localID] /= 2;
    }
  }

  if(localID == 0) {  // the root of the reduction subtree
    output[workgroupID] = reductionStds[0];
  }
  //printf("\nglobal id %d global size %d workgroupid %d local size %d local id %d transformedId %d\n", globalID,globalSize, workgroupID,localSize,localID, transformedId);
}
