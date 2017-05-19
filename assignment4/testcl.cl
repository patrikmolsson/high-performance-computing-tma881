__kernel void heat_diff( __global float* data, const uint rows, const uint cols, float c, uint iter){
  const uint transformedId = get_global_id(0) + (cols + 1) + 2 * (get_global_id(0) / (cols - 2));
  const uint i = transformedId / cols;
  const uint j = transformedId % cols;

  float h, h_l, h_r, h_u, h_d;
  h = data[(iter % 2) * cols * rows + i*cols+j];
  h_l = data[(iter % 2) * cols * rows + i*cols+j-1];
  h_r = data[(iter % 2) * cols * rows + i*cols+j+1];
  h_u = data[(iter % 2) * cols * rows + (i-1)*cols+j];
  h_d = data[(iter % 2) * cols * rows + (i+1)*cols+j];

  data[((iter + 1) % 2) * cols * rows + i * cols + j] = (1-c)*h+c*(h_l+h_r+h_u+h_d)/4;
}

__kernel void sum(__global const float *input, __global float *output, __local float *reductionSums, const uint cols){
  const uint globalID = get_global_id(0);
  const uint transformedId = globalID + (cols + 1) + 2 * (globalID / (cols - 2));
  const uint localID = get_local_id(0);
  const uint localSize = get_local_size(0);
  const uint workgroupID = globalID / localSize;

  reductionSums[localID] = input[transformedId];

  for(int offset = localSize >> 1; offset > 0; offset >>= 1){
    barrier(CLK_LOCAL_MEM_FENCE);
    if(localID < offset){
      reductionSums[localID] += reductionSums[localID + offset];
      reductionSums[localID] /= 2;
    }
  }

  if(localID == 0){
    output[workgroupID] = reductionSums[0];
  }
}

__kernel void std(__global const float *input, __global float *output, __local float *reductionStds, const uint cols, const float average){
  const uint globalID = get_global_id(0);
  const uint transformedId = globalID + (cols + 1) + 2 * (globalID / (cols - 2));
  const uint localID = get_local_id(0);
  const uint localSize = get_local_size(0);
  const uint workgroupID = globalID / localSize;

  reductionStds[localID] = fabs(input[transformedId] - average);

  for(int offset = localSize >> 1; offset > 0; offset >>= 1){
    barrier(CLK_LOCAL_MEM_FENCE);
    if(localID < offset){
      reductionStds[localID] += reductionStds[localID + offset];
      reductionStds[localID] /= 2;
    }
  }

  if(localID == 0){
    output[workgroupID] = reductionStds[0];
  }
}
