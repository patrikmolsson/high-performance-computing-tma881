#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

typedef struct{
  double complex root;
  int iter_conv;
  int type_conv;
} newton_res;

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


void newton_method(newton_res *result, const complex double x_init, const size_t d){
  int conv = -1;
  complex double x_0 = x_init;
  complex double x_1;
  complex double true_root[d];
  find_true_roots(d, true_root);
  size_t iter = 0;

  while(conv == -1 && cabs(x_0) > TOL_CONV && creal(x_0) < TOL_DIV && cimag(x_0 ) < TOL_DIV && iter < MAX_ITER ){
    x_1 = newton_iterate(x_0, d);
    for(size_t i=0; i<d;i++){
      if (cabs(x_1-true_root[i]) < TOL_CONV){
        //printf("x_1 re, %f tr re %f x_1 im %f tr im %f iter %ld dist: %f\n",creal(x_1), creal(true_root[i]), cimag(x_1), cimag(true_root[i]) ,iter,cabs(x_1-true_root[i]));
        conv = i;
      }
    }
    x_0 = x_1;
    iter++;
  }
  //result->root = x_1;
  //result->iter_conv = iter;
  (*result).root = x_1;
  (*result).iter_conv = iter;
  (*result).type_conv = conv;
}

void fill_grid(double complex ** grid, size_t grid_size, size_t interval){
  double complex incr;
  double d = 2*(double)interval / grid_size;
  d += d/(grid_size-1);
  incr = d + d*I;
  for (size_t i = 0; i < grid_size; i++){
    for (size_t j = 0; j < grid_size; j++){
      grid[i][j] = (j*creal(incr) - interval) + (i*cimag(incr)*I - interval*I);
      //printf("real %f imag %f \n",creal(grid[i][j]), cimag(grid[i][j]) );
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
    //printf("Color: %s \n", colormap[i] );
  }
}

void write_ppm(newton_res **sols, size_t grid_size, char **colormap, size_t d){
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
      type_of_conv = sols[i][j].type_conv;      
      if(type_of_conv >= 0 && type_of_conv <= d-1){
        strcat(for_print, colormap[type_of_conv]);
      } 
      else{
        strcat(for_print, "1 1 1 ");
      } 
    }    
    strcat(for_print, "\n");
    fprintf(fp, "%s", for_print);
  }
  fclose(fp);
}

int main(int argc, char *argv[]){
  size_t grid_size = 1000, interval = 2, d = 3, threads = 4; //Default values
  double complex ** grid;
  newton_res ** sols;
  char arg[10];

  if (argc > 4)
    printf("Too many arguments\n");

  for (size_t i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      memset(arg, 0, sizeof(arg));
      memcpy(arg, &argv[i][2], strlen(argv[i]) - 2);
      if (argv[i][1] == 'l') {
        // Grid size
        grid_size = (int) strtol(arg, NULL, 10);
      } else if (argv[i][1] == 't') {
        // Threads
        threads = (int) strtol(arg, NULL, 10);
      }
    }
    else if (isdigit(argv[i][0])) {
      // Digit => degrees
      d = (int) strtol(argv[i], NULL, 10);
    }
  }

  grid = malloc(grid_size * sizeof *grid);
  sols = malloc(grid_size * sizeof *sols);
  // COLOR MAP INIT
  const size_t str_length = 6;
  char ** colormap;
  colormap = malloc(d * sizeof *colormap);
  for(int i = 0; i < d; i++ ){
    colormap[i] = malloc( ( str_length + 1 ) );
  }
  root_color_map(colormap, d);
  for (size_t i=0; i < grid_size; i++){
    grid[i] = malloc(grid_size * sizeof *grid[i]);
    sols[i] = malloc(grid_size * sizeof *sols[i]);
  }

  fill_grid(grid, grid_size, interval);

  for (size_t i = 0; i < grid_size; i++){
    for (size_t j = 0; j < grid_size; j++){
      newton_method(&sols[i][j], grid[i][j], d);
      //printf("re %f im %f \n", creal(sols[i][j].root), cimag(sols[i][j].root));
    }
  }

  write_ppm(sols, grid_size, colormap, d);

  for (size_t i = 0; i < grid_size; i++){
    free(grid[i]);
    free(sols[i]);
  }

  free(grid);
  free(sols);
  for(int i = 0; i < d; i++ ){
    free(colormap[i]);
  }
  free(colormap);
  return 0;
}

