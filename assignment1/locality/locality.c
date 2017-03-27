#include <stdio.h>
#include <stdlib.h>

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
  //double test[10][10];

  size_t nrs = 1000, ncs = 1000, limit = 100;
  double **matrix;
  double *sums;

  // Allocate
  matrix = malloc(nrs * sizeof *matrix);
  sums = malloc(nrs * sizeof *sums);

  for (size_t i=0; i < nrs; i++){
    matrix[i] = malloc(ncs * sizeof *matrix[i]);
  }

  fill_matrix(matrix, nrs, ncs, limit);
  //fill_matrix(&test, 10, 10, limit);
  //col_sums(sums, matrix, nrs, ncs);

  // Deallocate
  for (size_t i = 0; i < nrs; i++){
    free(matrix[i]);
  }

  free(sums);
  free(matrix);


	return 0;

}