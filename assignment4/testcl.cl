__kernel void square( __global float* input, __global float* output, const unsigned int rows, unsigned int iter) {
  float h,h_l=0,h_r=0,h_u=0,h_d=0;
  int i = get_global_id(0) / rows;
  int j = get_global_id(0) % rows;
  int cols = rows; //FIXME: please
  float c = 1.0f/30; //FIXME: kill me

  if (iter % 2 == 0) {
    h = input[i*rows+j];
    if(j > 0)
      h_l = input[i*rows+j-1];
    if(j < cols-1)
      h_r = input[i*rows+j+1];
    if(i > 0)
      h_u = input[(i-1)*rows+j];
    if(i < rows-1)
      h_d = input[(i+1)*rows+j];
    output[i*rows+j] = (1-c)*h+c*(h_l+h_r+h_u+h_d)/4;

    //if(output[i*rows+j]>0.0f)
      //printf("input %f %d %d %lu\n",output[i*rows+j],i,j,iter);
  } else {
    h = output[i*rows+j];
    if(j > 0)
      h_l = output[i*rows+j-1];
    if(j < cols-1)
      h_r = output[i*rows+j+1];
    if(i > 0)
      h_u = output[(i-1)*rows+j];
    if(i < rows-1)
      h_d = output[(i+1)*rows+j];
    input[i*rows+j] = (1-c)*h+c*(h_l+h_r+h_u+h_d)/4;

    //if(input[i*rows+j]>0.0f)
      //printf("output %f %d %d %lu\n",input[i*rows+j],i,j,iter);
  }
}