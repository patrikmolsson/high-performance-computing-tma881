#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

// TODO: root not needed?
typedef struct{
  double complex root;
  int iter_conv;
  int type_conv;
} newton_res;

struct newton_method_args{
  newton_res *result;
  complex double *grid;
  complex double *true_roots;
};

static const double TOL_CONV = 1e-3;
static const double TOL_DIV = 10e10;
static const size_t MAX_ITER = 1e4;

// Init with default values
size_t d = 3;
size_t grid_size = 1000;
size_t num_threads = 3;
size_t block_size;

double complex newton_iterate(double complex x_0, double d){

  double complex x_1 = (1.0 - 1.0 /  d) * x_0 + ( 1.0 ) / (  d * cpow(x_0, d - 1) );

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
  struct newton_method_args *args = pv;
  complex double *grid = args->grid;
  newton_res* sols = args->result;
  complex double *true_roots = args->true_roots;

  complex double x_0, x_1;
  for(size_t i = 0; i<block_size; i++){
    for(size_t j = 0; j<grid_size; j++){
      x_0 = grid[i*grid_size+j];
      int conv = -1;
      size_t iter = 0;

      // TODO: Convergence: if abs(x_1) != 1 + eps, keep iterate.
      // Root check: start by checking real values <= always conjugate
      while(conv == -1
          && cabs(x_0) > TOL_CONV
          && creal(x_0) < TOL_DIV
          && cimag(x_0 ) < TOL_DIV
          && iter < MAX_ITER ){

        x_1 = newton_iterate(x_0,(double) d);

        if (fabs(1.0f - cabs(x_1)) < TOL_CONV ) {
          for(size_t i=0; i<d;i++){
            if (cabs(x_1-true_roots[i]) < TOL_CONV){
              conv = i;
            }
          }
        }
        x_0 = x_1;
        iter++;
      }
      sols[i*grid_size+j].iter_conv = iter;
      sols[i*grid_size+j].type_conv = conv;
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
      grid[i*grid_size + j] = (j*creal(incr) - interval) + (i*cimag(incr)*I - interval*I);
    }
  }
}

void root_color_map(char **colormap, size_t d){
  int k;
  for(int i=0; i<d; i++){
    for (int c = 2; c >= 0; c--){
    k = i >> c;
    if (k & 1){
      strcat( colormap[i], "1 " );
    }
    else
      strcat( colormap[i], "0 ");
    }
  }
}

void write_ppm_attractors(newton_res *sols, char **colormap){

  char str[25];
  sprintf(str, "newton_attractors_x%i.ppm", (int)d);
  FILE *fp;
  fp = fopen(str, "w+");

  fprintf(fp, "P3\n");
  fprintf(fp, "%ld %ld\n", grid_size, grid_size);
  fprintf(fp, "%d\n", 1);

  char for_print[grid_size * 6 + 1];
  int type_of_conv;

  size_t col = 1;

  for (size_t i = 0; i < grid_size * grid_size; i++){
    type_of_conv = sols[i].type_conv;

    if(type_of_conv >= 0 && type_of_conv <= d-1){
      strcat(for_print, colormap[type_of_conv]);
    }
    else{
      strcat(for_print, "1 1 1 ");
    }

    if (col % grid_size == 0) {
      strcat(for_print, "\n");
      fprintf(fp, "%s", for_print);
      memset(for_print, 0, grid_size * 6 + 1);
      col = 0;
    }
    col++;
  }

  fclose(fp);
}

void write_ppm_convergence(newton_res *sols, char **colormap){
  char str[26];
  sprintf(str, "newton_convergence_x%i.pgm", (int)d);

  FILE *fp;
  fp = fopen(str, "w+");

  fprintf(fp, "P2\n");
  fprintf(fp, "%ld %ld\n", grid_size, grid_size);
  fprintf(fp, "%d\n", 255);

  char for_print[grid_size * 4 + 1];
  int iter_conv;

  size_t z = 1;
  char str2[10];

  for (size_t i = 0; i < grid_size * grid_size; i++){
    iter_conv = sols[i].iter_conv;

    sprintf(str2, "%i ", iter_conv * 10);

    strcat(for_print, str2);

    memset(str2, 0, sizeof str2);

    if (z % grid_size == 0) {
      strcat(for_print, "\n");
      fprintf(fp, "%s", for_print);
      memset(for_print, 0, sizeof for_print);
      z = 0;
    }
    z++;
  }

  fclose(fp);
}

int main(int argc, char *argv[]){
  size_t interval = 2;
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

  // COLOR MAP INIT
  const size_t str_length = 6;
  char ** colormap;
  colormap = malloc(d * sizeof *colormap);
  for(int i = 0; i < d; i++ ){
    colormap[i] = malloc( ( str_length + 1 ) );
  }
  root_color_map(colormap, d);
  grid = malloc(grid_size * grid_size * sizeof *grid);
  sols = malloc(grid_size * grid_size * sizeof *sols);

  fill_grid(grid, grid_size, interval);

  // Divide the grid's rows into num_threads st block. Pass starting point of a block to each thread. Not guaranteed to be integer => Do int division, last thread takes the remaining row (for loop down below).

  block_size = grid_size / num_threads;

  complex double true_roots[d];
  find_true_roots(d, true_roots);

  if(num_threads > 1) {
    pthread_t threads[num_threads];

    struct newton_method_args *args = malloc(num_threads * sizeof (struct newton_method_args));

    int rc;
    size_t t,ix; //Wanted cool double index, seems to require external prealloc.
    for (t = 0, ix = 0; t < num_threads; t++, ix += block_size){
      args[t].result = &sols[ix*grid_size]; // Send in pointers to first element in grid and sols blocks and then access all other elements relative to the starting value.
      args[t].true_roots = true_roots;
      args[t].grid = &grid[ix*grid_size];
      rc = pthread_create(&threads[t], NULL, &newton_method, &args[t]);
      if(rc) {
        fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
          return 1;
      }
    }

    for (t = 0; t < num_threads; t++){
      rc = pthread_join(threads[t], NULL);
      if(rc)
        fprintf(stderr, "error: pthread_join, rc: %d \n", rc);
    }
    free(args);
  } else {
    struct newton_method_args args;
    args.result = &sols[0];
    args.grid = &grid[0];
    args.true_roots = true_roots;
    newton_method((void *) &args);
  }

  write_ppm_attractors(sols, colormap);
  write_ppm_convergence(sols, colormap);

  free(grid);
  free(sols);
  for(int i = 0; i < d; i++ ){
    free(colormap[i]);
  }
  free(colormap);
  //pthread_exit(NULL);
  return 0;
}
