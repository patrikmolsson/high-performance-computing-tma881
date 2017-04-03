#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

size_t rand_lim(size_t limit) {
/* return a random number between 0 and limit inclusive.
 */
  // TODO: Eventually add explicit random seed(using time or smth)
  size_t divisor = RAND_MAX/(limit+1);
  size_t retval;

  do {
    retval = rand() / divisor;
  } while (retval > limit);

  return retval;
}

//Changed from **matrix to matrix[][], is this ok?
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

void fill_matrix(double ** matrix, size_t nrs, size_t ncs, size_t limit){
  for ( size_t ix=0; ix < nrs; ++ix ) {
    for ( size_t jx=0; jx < ncs; ++jx ){
      double tmp = rand_lim(limit);
      matrix[ix][jx] = tmp;
      //printf("Assigned value %f\n", matrix[ix][jx]);
    }
  }
}

int main(){
  size_t nrs = 1000, ncs = 1000, limit = 100;
  double **matrix;
  double *sums;
  double *sums2;
  double tol = 0.001;

  // Allocate

  matrix = malloc(nrs * sizeof *matrix);
  sums = malloc(nrs * sizeof *sums);
  sums2 = malloc(nrs * sizeof *sums);

  for (size_t i=0; i < nrs; i++){
    matrix[i] = malloc(ncs * sizeof *matrix[i]);
  }

  fill_matrix(matrix, nrs, ncs, limit);

  // Not sure if we should time here or use the makefile thingy

  clock_t begin = clock();
  row_sums(sums, (const double **)matrix, nrs, ncs);
  clock_t end = clock();
  double time_row_sums = (double)(end - begin) / CLOCKS_PER_SEC;

  begin = clock();
  col_sums(sums, (const double **)matrix, nrs, ncs);
  end = clock();
  double time_col_sums = (double)(end - begin) / CLOCKS_PER_SEC;

  begin = clock();
  col_sums2(sums2, (const double **)matrix, nrs, ncs);
  end = clock();
  double time_col_sums2 = (double)(end - begin) / CLOCKS_PER_SEC;

  int same = check_col_sums(sums, sums2, nrs, tol);

  printf("colsums time: %f colsums2 time: %f rowsums time: %f \n", time_col_sums, time_col_sums2, time_row_sums);
  printf("%s %d\n","Are colsum and colsum2 the same?", same);

  //Deallocate

  for (size_t i = 0; i < nrs; i++){
    free(matrix[i]);
  }

  free(sums);
  free(sums2);
  free(matrix);

	return 0;
}
