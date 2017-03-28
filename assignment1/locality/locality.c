#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
void row_sums(size_t nrs, size_t ncs,double sums[nrs], const double matrix[nrs][ncs]){
  for ( size_t ix=0; ix < nrs; ++ix ) {
    double sum = 0;
    for ( size_t jx=0; jx < ncs; ++jx )
      sum += matrix[ix][jx];
    sums[ix] = sum;
  }
}

void col_sums(size_t nrs, size_t ncs, double sums[nrs], const double matrix[nrs][ncs]){
  for ( size_t jx=0; jx < ncs; ++jx ) {
    double sum = 0;
    for ( size_t ix=0; ix < nrs; ++ix )
      sum += matrix[ix][jx];
    sums[jx] = sum;
  }
}

void fill_matrix(size_t nrs, size_t ncs, double matrix[nrs][ncs], size_t limit){
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
  double matrix[nrs][ncs];
  double sums[nrs];

  fill_matrix(nrs, ncs, matrix, limit);

  // Not sure if we should time here or use the makefile thingy
  clock_t begin = clock();
  col_sums(nrs, ncs, sums, matrix);
  clock_t end = clock();
  double time_col_sums = (double)(end - begin) / CLOCKS_PER_SEC;

  row_sums(nrs, ncs, sums, matrix);
  begin = clock();
  row_sums(nrs, ncs, sums, matrix);
  end = clock();
  double time_row_sums = (double)(end - begin) / CLOCKS_PER_SEC;

  printf("colsums: %f rowsums: %f \n", time_col_sums, time_row_sums);

	return 0;
}
