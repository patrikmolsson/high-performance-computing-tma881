__kernel void square( __global float* data, const unsigned int rows, unsigned int iter) {
  float h, h_l=0, h_r=0, h_u=0, h_d=0;
  int i = get_global_id(0) / rows;
  int j = get_global_id(0) % rows;
  int cols = rows; //FIXME: please
  float c = 1.0f/30; //FIXME: kill me

  h = data[(iter % 2) * cols * rows + i*rows+j];
  if(j > 0)
    h_l = data[(iter % 2) * cols * rows + i*rows+j-1];
  if(j < cols-1)
    h_r = data[(iter % 2) * cols * rows + i*rows+j+1];
  if(i > 0)
    h_u = data[(iter % 2) * cols * rows + (i-1)*rows+j];
  if(i < rows-1)
    h_d = data[(iter % 2) * cols * rows + (i+1)*rows+j];
  data[((iter + 1) % 2) * cols * rows + i * rows + j] = (1-c)*h+c*(h_l+h_r+h_u+h_d)/4;
}
