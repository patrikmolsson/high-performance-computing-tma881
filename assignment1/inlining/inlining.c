#include <stdio.h>
#include "inliningSep.c"
#include <stdlib.h>
#include <time.h>

int rand_lim(int limit) {
/* return a random number between 0 and limit inclusive.
 */
  // TODO: Eventually add explicit random seed(using time or smth)
  int divisor = RAND_MAX/(limit+1);
  int retval;

  do {
    retval = rand() / divisor;
  } while (retval > limit);

  return retval;
}

void rand_n_nbrs(int * rnd_array, int n, int limit){
  for (int i = 0; i < n; i++){
    rnd_array[i] = rand_lim(limit);
  }
}

void initialize_arrays(double * a_re, double * a_im, double * b_re, double * b_im, double * c_re, double * c_im, int length_arr, int limit){
  int n_arrays = 4;
  for (int i = 0; i < length_arr; i++){
    int rnd_array[n_arrays];
    rand_n_nbrs(rnd_array, n_arrays, limit);
    a_re[i] = 0; // Probably not needed since we set the values later
    a_im[i] = 0;
    b_re[i] = rnd_array[0];
    b_im[i] = rnd_array[1];
    c_re[i] = rnd_array[2];
    c_im[i] = rnd_array[3];
  }
}

void mul_cpx(double * a_re, double * a_im, double * b_re, double * b_im, double * c_re, double * c_im){
  (*a_re) = (*b_re) * (*c_re);
  (*a_im) = (*b_im) * (*c_im);
}

void multiply_arrays(double * a_re, double * a_im, double * b_re, double * b_im, double * c_re, double * c_im, int length_arr){
  for (int i = 0; i < length_arr; i++){
    //printf("before: a_re %f a_im %f b_re %f b_im %f c_re %f c_im %f \n", a_re[i], a_im[i], b_re[i], b_im[i], c_re[i], c_im[i]);
    mul_cpx(&a_re[i], &a_im[i], &b_re[i], &b_im[i], &c_re[i], &c_im[i]);
    //printf("after: a_re %f a_im %f b_re %f b_im %f c_re %f c_im %f \n", a_re[i], a_im[i], b_re[i], b_im[i], c_re[i], c_im[i]);
  }
}

void multiply_arrays_sep(double * a_re, double * a_im, double * b_re, double * b_im, double * c_re, double * c_im, int length_arr){
  for (int i = 0; i < length_arr; i++){
    mul_cpx_sep(&a_re[i], &a_im[i], &b_re[i], &b_im[i], &c_re[i], &c_im[i]);
  }
}

void multiply_arrays_man_inline(double * a_re, double * a_im, double * b_re, double * b_im, double * c_re, double * c_im, int length_arr){
  for (int i = 0; i < length_arr; i++){
    a_re[i] = b_re[i] * c_re[i];
    a_im[i] = b_im[i] * c_im[i];
  }
}

int main(){
  int length_arr = 30000;
  int limit = 20;
  double a_re[length_arr];
  double a_im[length_arr];
  double b_re[length_arr];
  double b_im[length_arr];
  double c_re[length_arr];
  double c_im[length_arr];
  initialize_arrays(a_re, a_im, b_re, b_im, c_re, c_im, length_arr, limit);

  clock_t begin = clock();
  multiply_arrays(a_re, a_im, b_re, b_im, c_re, c_im, length_arr);
  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("%s %f \n","Time for mul_cpx: \n", time_spent);

  begin = clock();
  multiply_arrays_sep(a_re, a_im, b_re, b_im, c_re, c_im, length_arr);
  end = clock();
  time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("%s %f \n","Time for mul_cpx_sep: \n", time_spent);

  begin = clock();
  multiply_arrays_man_inline(a_re, a_im, b_re, b_im, c_re, c_im, length_arr);
  end = clock();
  time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("%s %f \n","Time for mul_cpx_inline: \n", time_spent);

  return 0;
}
