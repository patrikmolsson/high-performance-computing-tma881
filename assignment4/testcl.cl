__kernel void square( __global float* data, const unsigned int rows, unsigned int iter) {
  float h, h_l, h_r, h_u, h_d;
  int cols = rows; //FIXME: please
  float c = 1.0f/30; //FIXME: kill me

  // We transform the indices to match the padded vector
  int transformedId = get_global_id(0) + (cols + 1) + 2 * (get_global_id(0) / (cols - 2));

  int i = transformedId / rows;
  int j = transformedId % rows;

  h = data[(iter % 2) * cols * rows + i*rows+j];
  h_l = data[(iter % 2) * cols * rows + i*rows+j-1];
  h_r = data[(iter % 2) * cols * rows + i*rows+j+1];
  h_u = data[(iter % 2) * cols * rows + (i-1)*rows+j];
  h_d = data[(iter % 2) * cols * rows + (i+1)*rows+j];

  data[((iter + 1) % 2) * cols * rows + i * rows + j] = (1-c)*h+c*(h_l+h_r+h_u+h_d)/4;
}
