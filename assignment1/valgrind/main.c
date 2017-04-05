#include <stdlib.h>

void allocate_array(size_t n){
  int *nelson_mandela;
  nelson_mandela = (int *) malloc (sizeof(int) * n);
  // free(nelson_mandela);
}

int main(){
  size_t n = 1000;
  allocate_array(n);
}

