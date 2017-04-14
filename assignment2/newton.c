#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <ctype.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
// TODO: root not needed?
typedef struct{
  double complex root;
  int iter_conv;
  int type_conv;
} newton_res;

struct newton_method_args{
  /*newton_res *result;
  complex double x_init;
  size_t d;
  long tid;*/

  size_t d;
  newton_res *result;
  complex double *grid;
  long tid;
  size_t grid_size;
  size_t block_size;
};

static const double TOL_CONV = 1e-3;
static const double TOL_DIV = 10e10;
static const double MAX_ITER = 1e4;

double complex newton_iterate(double complex x_0, size_t d){

  double complex x_1 = x_0 -( cpow(x_0,d) - 1.0) / ( (double) d * cpow(x_0,d-1) );

  return x_1;
}

void find_true_roots(size_t d, complex double *true_root){
  /* One class of polynom: x^d - 1 = 0; => (a) one root always Re(root) = 1; (b) complex roots conjugate  */
  true_root[0] = 1.0;
  for(size_t i=1; i<d; i++ ){
    double arg = i*2*M_PI/d;
    true_root[i] = cos(arg) + sin(arg)*I;
  }
}


void * newton_method(void * pv){
  //newton_res *result, const complex double x_init, const size_t d
  struct newton_method_args *args = pv;
  size_t d = args->d;
  size_t block_size = args->block_size;
  size_t grid_size = args ->grid_size;
  complex double true_root[d];
  find_true_roots(d, true_root);
  complex double *grid = args->grid;
  newton_res *sols = args->result;

  complex double x_0, x_1;
  for(size_t i = 0; i<block_size; i++){
    for(size_t j = 0; j<grid_size; j++){
      x_0 = grid[i*grid_size+j];
      int conv = -1;
      size_t iter = 0;

      // TODO: Convergence: if abs(x_1) != 1 + eps, keep iterate.
      // Root check: start by checking real values <= always conjugate
      while(conv == -1 && cabs(x_0) > TOL_CONV && creal(x_0) < TOL_DIV && cimag(x_0 ) < TOL_DIV && iter < MAX_ITER ){
        x_1 = newton_iterate(x_0, d);
        for(size_t i=0; i<d;i++){
          if (cabs(x_1-true_root[i]) < TOL_CONV){
            conv = i;
          	//printf("x_1 re, %f tr re %f x_1 im %f tr im %f iter %ld dist: %f i %ld\n",creal(x_1), creal(true_root[i]), cimag(x_1), cimag(true_root[i]) ,iter,cabs(x_1-true_root[i]),i);
          }
        }
        x_0 = x_1;
        iter++;
      }
      sols[i*grid_size+j].iter_conv = iter;
      sols[i*grid_size+j].type_conv = conv;
      //args->result[i*grid_size+j].iter_conv = iter;
      //args->result[i*grid_size+j].type_conv = conv;
    }
  }
  return NULL;
}

void fill_grid(double complex * grid, size_t grid_size, size_t interval){
  double complex incr;
  double d = 2*(double)interval / grid_size;
  d += d/(grid_size-1);
  incr = d + d*I;
  for (size_t i = 0; i < grid_size; i++){
    for (size_t j = 0; j < grid_size; j++){
      grid[i + j*grid_size] = (j*creal(incr) - interval) + (i*cimag(incr)*I - interval*I);
      //printf("real %f imag %f \n",creal(grid[i][j]), cimag(grid[i][j]) );
    }
  }
}

void write_ppm(newton_res *sols, size_t grid_size){
  FILE *fp;
  fp = fopen("test.ppm", "w+");
  char for_print[6*grid_size + 1];
  int type_of_conv;
  fprintf(fp, "P3\n");
  fprintf(fp, "%ld %ld\n", grid_size, grid_size);
  fprintf(fp, "%d\n", 1);
  for (size_t i = 0; i < grid_size; i++){
    memset(for_print, 0, sizeof(for_print));
    for (size_t j = 0; j < grid_size; j++){
      type_of_conv = sols[i + j*grid_size].type_conv;
      if(type_of_conv == 0){
        strcat(for_print, "1 0 0 ");
      } else if(type_of_conv == 1){
        strcat(for_print, "0 1 0 ");
      } else if(type_of_conv == 2){
        strcat(for_print, "0 0 1 ");
      }
    }
    strcat(for_print, "\n");
    fprintf(fp, "%s", for_print);
  }
  fclose(fp);
}

int main(int argc, char *argv[]){
  size_t grid_size = 200, interval = 2, d = 3, num_threads = 4; //Default values
  double complex * grid;
  newton_res * sols;
  char arg[10];

  if (argc > 4)
    printf("Too many arguments\n");

  for (size_t i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      memset(arg, 0, sizeof(arg));
      memcpy(arg, &argv[i][2], strlen(argv[i]) - 2);
      if (argv[i][1] == 'l') {
        // Grid size
        grid_size = (size_t) strtol(arg, NULL, 10);
      } else if (argv[i][1] == 't') {
        // Threads
        num_threads = (size_t) strtol(arg, NULL, 10);
      }
    }
    else if (isdigit(argv[i][0])) {
      // Digit => degrees
      d = (int) strtol(argv[i], NULL, 10);
    }
  }

  grid = malloc(grid_size * grid_size * sizeof *grid);
  sols = malloc(grid_size * grid_size * sizeof *sols);

  fill_grid(grid, grid_size, interval);

  // Divide the grid's rows into num_threads st block. Pass starting point of a block to each thread. Not guaranteed to be integer => Do int division, last thread takes the remaining row (for loop down below).
  size_t  block_size = grid_size / num_threads;
  pthread_t threads[num_threads];
  int rc;
  struct newton_method_args args; //Don't need array of args, overwrite before each create thread
  args.d = d; // d, and grid_size: No overwrite, same for all threads, glob varibales instead perhaps, but we get them from main args... block_size may differ for last thread.
  args.grid_size= grid_size;
  args.block_size= block_size;
  size_t t,ix; //Wanted cool double index, seems to require external prealloc.
  for (t = 0, ix = 0; t < num_threads; t++, ix += block_size){
    args.tid = (long)t;
    args.result = &sols[ix*grid_size]; // Send in pointers to first element in grid and sols blocks and then access all other elements relative to the starting value.
    args.grid = &grid[ix*grid_size];
    rc = pthread_create(&threads[t], NULL, newton_method, &args);
    printf("ix t %zu %zu\n",ix, t);
    if(rc) {
      fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
        return 1;
      }
    }
  for (int i = 0; i < num_threads; ++i) {
    rc = pthread_join(threads[i], NULL);
    if(rc)
      fprintf(stderr, "error: pthread_join, rc: %d \n", rc);
  }

  write_ppm(sols, grid_size);

  free(grid);
  free(sols);
  return 0;
}
