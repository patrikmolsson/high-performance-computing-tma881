#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "inliningSep.h"

typedef void (*FUNC_PTR)(double *, double *, double *, double *, double *, double *, size_t);

int rand_lim(int limit) {
/* return a random number between 0 and limit inclusive.
 */
  int divisor = RAND_MAX/(limit+1);
  int retval;

  do {
    retval = rand() / divisor;
  } while (retval > limit);

  return retval;
}

void rand_n_nbrs(int * rnd_array, size_t n, int limit){
  for (size_t i = 0; i < n; i++){
    rnd_array[i] = rand_lim(limit);
  }
}

void initialize_arrays(double * a_re, double * a_im, double * b_re, double * b_im, double * c_re, double * c_im, size_t length_arr, int limit){
  size_t n_arrays = 4;
  for (size_t i = 0; i < length_arr; i++){
    int rnd_array[n_arrays];
    rand_n_nbrs(rnd_array, n_arrays, limit);
    a_re[i] = 0;
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

void multiply_arrays(double * a_re, double * a_im, double * b_re, double * b_im, double * c_re, double * c_im, size_t length_arr){
  for (size_t i = 0; i < length_arr; i++){
    mul_cpx(&a_re[i], &a_im[i], &b_re[i], &b_im[i], &c_re[i], &c_im[i]);
  }
}

void multiply_arrays_sep(double * a_re, double * a_im, double * b_re, double * b_im, double * c_re, double * c_im, size_t length_arr){
  for (size_t i = 0; i < length_arr; i++){
    mul_cpx_sep(&a_re[i], &a_im[i], &b_re[i], &b_im[i], &c_re[i], &c_im[i]);
  }
}

void multiply_arrays_man_inline(double * a_re, double * a_im, double * b_re, double * b_im, double * c_re, double * c_im, size_t length_arr){
  for (size_t i = 0; i < length_arr; i++){
    a_re[i] = b_re[i] * c_re[i];
    a_im[i] = b_im[i] * c_im[i];
  }
}

static double timespec_to_seconds (struct timespec* ts){
  return (double)ts -> tv_sec + (double)ts -> tv_nsec / 1000000000.0;
}

double benchmark_function(double * a_re, double * a_im, double * b_re, double * b_im, double * c_re, double * c_im, size_t length_arr, FUNC_PTR func, const size_t reps){
  struct timespec start;
  struct timespec end;
  double elapsed_seconds = 0;

for(size_t i = 0; i < reps; ++i){
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);
  func(a_re, a_im, b_re, b_im, c_re, c_im, length_arr);
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  elapsed_seconds += timespec_to_seconds(&end) - timespec_to_seconds(&start);
}
  return elapsed_seconds;
}

int main(){
  size_t length_arr = 30000;
  const size_t reps = 100;
  int limit = 20;
  double a_re[length_arr];
  double a_im[length_arr];
  double b_re[length_arr];
  double b_im[length_arr];
  double c_re[length_arr];
  double c_im[length_arr];
  initialize_arrays(a_re, a_im, b_re, b_im, c_re, c_im, length_arr, limit);

  double time_cpx = benchmark_function(a_re, a_im, b_re, b_im, c_re, c_im, length_arr, multiply_arrays, reps);
  printf("%s %.9g \n","Time for mul_cpx: \n", time_cpx);

  double time_cpx_sep = benchmark_function(a_re, a_im, b_re, b_im, c_re, c_im, length_arr, multiply_arrays_sep, reps);
  printf("%s %.9g \n","Time for mul_cpx_sep: \n", time_cpx_sep);

  double time_man_inline = benchmark_function(a_re, a_im, b_re, b_im, c_re, c_im, length_arr, multiply_arrays_man_inline, reps);
  printf("%s %.9g \n","Time for mul_cpx_inline: \n", time_man_inline);

  return 0;
}
