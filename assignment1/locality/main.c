#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef void (*FUNC_PTR)(double *, const double **, size_t, size_t);

size_t rand_lim(size_t limit) {
/* return a random number between 0 and limit inclusive.
 */
  size_t divisor = RAND_MAX/(limit+1);
  size_t retval;

  do {
    retval = rand() / divisor;
  } while (retval > limit);

  return retval;
}

void row_sums(double * sums, const double ** matrix, size_t nrs, size_t ncs){
  for ( size_t ix=0; ix < nrs; ++ix ) {
    double sum = 0;
    for ( size_t jx=0; jx < ncs; ++jx )
      sum += matrix[ix][jx];
    sums[ix] = sum;
  }
}

void col_sums(double * sums, const double ** matrix, size_t nrs, size_t ncs){
  for ( size_t jx=0; jx < ncs; ++jx ) {
    double sum = 0;
    for ( size_t ix=0; ix < nrs; ++ix )
      sum += matrix[ix][jx];
    sums[jx] = sum;
  }
}

void col_sums2(double * sums, const double ** matrix, size_t nrs, size_t ncs){
  for ( size_t ix=0; ix < nrs; ++ix ){
    for ( size_t jx=0; jx < ncs; ++jx ) {
        sums[jx] += matrix[ix][jx];
    }
  }
}

int check_col_sums(double * sums, double * sums2, size_t nrs,  double tol){
  int same = 1;
  for ( size_t ix=0; ix < nrs; ++ix ){
    double diff = sums[ix] - sums2[ix];
    if ( fabs(diff) > tol ){
      printf("sum %f sum2 %f \n", sums[ix], sums2[ix]);
      return same = 0;
    }
  }
  return same;
}

void fill_matrices(double ** matrix, double * sums1, double * sums2, double * sums3, size_t nrs, size_t ncs, size_t limit){
  for ( size_t ix=0; ix < nrs; ++ix ) {
    sums1[ix] = 0;
    sums3[ix] = 0;
    sums3[ix] = 0;
    for ( size_t jx=0; jx < ncs; ++jx ){
      double tmp = rand_lim(limit);
      matrix[ix][jx] = tmp;
    }
  }
}

double benchmark_function(double * sums, const double ** matrix, size_t nrs, size_t ncs, FUNC_PTR func, const size_t reps){
  struct timespec start;
  struct timespec end;
  double elapsed_seconds = 0;

  for(size_t i = 0; i < reps; ++i){
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    func(a_re, a_im, b_re, b_im, c_re, c_im, length_arr);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    elapsed_seconds += timespec_to_seconds(&end) - timespec_to_seconds(&start);
  }
    return elapsed_seconds / reps;
}

int main(){
  size_t nrs = 1000, ncs = 1000, limit = 100;
  double **matrix;
  double *sums1;
  double *sums2;
  double *sums3;
  double tol = 0.001;
  size_t reps = 100;


  // Allocate

  matrix = malloc(nrs * sizeof *matrix);
  sums1 = malloc(nrs * sizeof *sums1);
  sums2 = malloc(nrs * sizeof *sums2);
  sums3 = malloc(nrs * sizeof *sums3);

  for (size_t i=0; i < nrs; i++){
    matrix[i] = malloc(ncs * sizeof *matrix[i]);
  }

  fill_matrices(matrix, sums1, sums2, sums3, nrs, ncs, limit);

  double time_col_sums = benchmark_function(sums1, (const double **)matrix, nrs, ncs, col_sums, reps);
  double time_col_sums2 = benchmark_function(sums2, (const double **)matrix, nrs, ncs, col_sums2, reps);
  double time_row_sums = benchmark_function(sums3, (const double **)matrix, nrs, ncs, row_sums, reps);

  int same = check_col_sums(sums1, sums2, nrs, tol);

  printf("colsums time: %.9g colsums2 time: %.9g rowsums time: %.9g \n", time_col_sums, time_col_sums2, time_row_sums);
  printf("%s %d\n","Are colsum and colsum2 the same?", same);

  //Deallocate

  for (size_t i = 0; i < nrs; i++){
    free(matrix[i]);
  }

  free(sums1);
  free(sums2);
  free(sums3);
  free(matrix);

	return 0;
}
