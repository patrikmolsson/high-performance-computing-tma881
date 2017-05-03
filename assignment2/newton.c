#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

typedef struct{
  int iter_conv;
  int type_conv;
} newton_res;

struct newton_method_args{
  size_t ix;
  char **colormap;
  char *for_print_attr;
  char *for_print_conv;
  double **true_roots;
};

struct write_method_args{
  FILE *fp_attr;
  FILE *fp_conv;
  char **for_print_attr;
  char **for_print_conv;
};

static const double TOL_CONV = 1e-3;
static const double TOL_DIV = 10e10;
static const size_t CHUNK_SIZE = 50e5;
static const size_t interval = 2;
//static const size_t MAX_ITER = 999;

// Init with default valuesm
size_t n_chunks;
size_t d = 3;
size_t grid_size = 1000;
size_t num_threads = 3;
size_t block_size;
void (*newton_pointer)( double*, double*);
size_t max_iters_deg[8] = {1,14,49,185,123,197,290,450};
size_t max_iter;

int running_thread_write = 0;


void newton_iterate1(double *x0_re, double *x0_im){

  *x0_re = 1; 
  *x0_im = 0;
}

void newton_iterate2(double *x0_re, double *x0_im){

  double r_2 = *x0_re* *x0_re + *x0_im * *x0_im;
  *x0_re = *x0_re * ( 1 + 1.0f/r_2 ) / 2 ;

  *x0_im = *x0_im * (1 - 1.0f / r_2 ) / 2 ;
}

void newton_iterate(double *x0_re, double *x0_im){

  // atan2; ensuring principal branch for arg(z).
  double arg = - atan2(*x0_im,*x0_re) *  (d - 1.0f);
  // Magnitude for 1/ ( d x^(d-1) )
  double r_2 = pow( *x0_re* *x0_re + *x0_im * *x0_im , (1.0f-d)/2 ) / ( d*1.0f )  ;
  *x0_re = (1 - 1.0f / d) * *x0_re + r_2 * cos(arg);
  *x0_im = (1 - 1.0f / d) * *x0_im + r_2 * sin(arg);
  // Previous complex double version for reference:
  //*x_0 = (1.0f - 1.0f / d) * *x_0 + ( 1.0 ) / (  d*1.0f * cpow(*x_0, d - 1) );
}

void find_true_roots(double ** true_root){
  /* One class of polynom: x^d - 1 = 0; => (a) one root always Re(root) = 1; (b) complex roots conjugate  */
  true_root[0][0] = 1.0f;
  true_root[0][1] = 0.0f;
  for(size_t i=1; i<d; i++ ){
    double arg = i*2*M_PI/d;
    true_root[i][0] = cos(arg);
    true_root[i][1] = sin(arg);
  }
}

void * newton_method(void * pv){
  struct newton_method_args *args = pv;
  double **true_roots = args->true_roots;
  char *colstr;
  char *for_print_attr  = args->for_print_attr;
  char *for_print_conv = args->for_print_conv;
  char **colormap = args->colormap;
  size_t  iter = 0, ix = args->ix;
  double x0_re, x0_im, x_abs;
  double incr = 2*(double)interval / grid_size;
  incr += incr/(grid_size-1);
  int conv;

  for(size_t i = 0; i<block_size; i++){
    for(size_t j = 0; j<grid_size; j++){
      x0_re = j*incr - interval;
      x0_im = (i+ix)*incr - interval;
      conv = -1;
      iter = 0;
      x_abs = sqrt(x0_re*x0_re+x0_im*x0_im);

      // TODO: Convergence: if abs(x_1) != 1 + eps, keep iterate.
      // Root check: start by checking real values <= always conjugate
      while(conv == -1
        && x_abs > TOL_CONV
        && fabs(x0_re) < TOL_DIV
        && fabs(x0_im) < TOL_DIV ){
        (*newton_pointer)(&x0_re,&x0_im);
        //newton_iterate1(&x0_re,&x0_im);  
        x_abs = sqrt(x0_re*x0_re+x0_im*x0_im);

        if (fabs(1.0f - x_abs) < TOL_CONV ) {
          for(size_t k=0; k<d;k++){
            if (fabs(pow(x0_re-true_roots[k][0],2) + pow(x0_im-true_roots[k][1],2))  < TOL_CONV){
              conv = k;
            }
          }
        }
        iter++;
      }

      if (conv > -1)
        colstr = colormap[conv];
      else
        colstr = "1 1 1 ";

      if(iter > max_iter)
        iter = max_iter;

      for (size_t k = 0; k < 6; k++){
        for_print_attr[i*(grid_size*6+1)+j*6+k] = colstr[k];
      }
      
      sprintf(&for_print_conv[i*(grid_size*4+1)+j*4], "%-4lu ", iter);
      if ((j+1) % grid_size == 0) {
        for_print_attr[i*(grid_size*6+1)+j*6+6] = '\n';
        for_print_conv[i*(grid_size*4+1)+j*4+4] = '\n';
      }
    }
  }
  return NULL;
}


void * write_method(void * pv){
  struct write_method_args *args = pv;
  char **for_print_attr = args->for_print_attr;
  char **for_print_conv = args->for_print_conv;
  FILE *fp_attr = args->fp_attr;
  FILE *fp_conv = args->fp_conv;
  // TODO Free pv and args? NOT pv! It is being used later

  for (size_t i = 0; i < num_threads; i++) {
    fprintf(fp_attr, "%s", for_print_attr[i]);
    fprintf(fp_conv, "%s", for_print_conv[i]);
  }

  return NULL;
}

void root_color_map(char **colormap){
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

int main(int argc, char *argv[]){
  double ** true_roots;
  char ** colormap;
  char *** for_print_attr;
  char *** for_print_conv;

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

  const size_t str_length = 6;
  //colormap = malloc(d * sizeof *colormap);
  colormap = calloc(d, sizeof *colormap);
  true_roots = malloc(d * sizeof *true_roots);

  for(int i = 0; i < d; i++ ){
    colormap[i] = calloc(str_length + 1, sizeof *colormap[i]);
    true_roots[i] = malloc(2 * sizeof *true_roots[i]);
  }
  root_color_map(colormap);

  // Set function newton iterate to simple hardcoded solutions if d = {1,2}, standard else:

  if ( d == 1  ){
    newton_pointer = &newton_iterate1;
  }
  else if ( d == 2 ){
    newton_pointer = &newton_iterate2;
  }
  else{
    newton_pointer = &newton_iterate;
  }

  max_iter = max_iters_deg[d-1];
  if ( grid_size == 50000 ){
    max_iter = max_iters_deg[7];
  }
  // Divide the grid's rows into num_threads st block. Pass starting point of a block to each thread. Not guaranteed to be integer => Do int division, last thread takes the remaining row (for loop down below).
  n_chunks = ceil((float) grid_size*(grid_size * 6 + 1) / CHUNK_SIZE);

  block_size = ceil((float) grid_size / (n_chunks * num_threads));

  if (n_chunks * num_threads * block_size > grid_size) {
    n_chunks = ceil((float)grid_size / (num_threads * block_size));
  }
  //printf("nchunk %lu block_size %lu \n",n_chunks,block_size);

  find_true_roots(true_roots);

  size_t hasRemainder = 0;

  if ((n_chunks * grid_size) % block_size > 0)
    hasRemainder = 1;

  char str[26];
  sprintf(str, "newton_attractors_x%i.ppm", (int)d);
  FILE *fp_attr = fopen(str,"w+");
  fprintf(fp_attr, "P3\n");
  fprintf(fp_attr, "%ld %ld\n", grid_size, grid_size);
  fprintf(fp_attr, "%d\n", 1);

  sprintf(str, "newton_convergence_x%i.ppm", (int)d);
  FILE *fp_conv = fopen(str, "w+");
  fprintf(fp_conv, "P2\n");
  fprintf(fp_conv, "%ld %ld\n", grid_size, grid_size);
  fprintf(fp_conv, "%zu\n", max_iter);

  pthread_t threads[num_threads];
  pthread_t write_thread;
  struct newton_method_args *args = malloc(num_threads * sizeof (struct newton_method_args));
  struct write_method_args write_args;

  write_args.fp_attr = fp_attr;
  write_args.fp_conv = fp_conv;

  int rc;
  size_t t,ix; //Wanted cool double index, seems to require external prealloc.
  for_print_attr = malloc(2 * sizeof(char**));
  for_print_attr[0] = malloc(num_threads * sizeof(char*));
  for_print_attr[1] = malloc(num_threads * sizeof(char*));
  for_print_conv = malloc(2 * sizeof(char**));
  for_print_conv[0] = malloc(num_threads * sizeof(char*));
  for_print_conv[1] = malloc(num_threads * sizeof(char*));
  for(size_t j = 0; j < num_threads; j++){
    for_print_attr[0][j] = malloc(grid_size * (block_size + 1) * 6);
    for_print_attr[1][j] = malloc(grid_size * (block_size + 1) * 6);
    for_print_conv[0][j] = malloc(grid_size * (block_size + 1) * 4);
    for_print_conv[1][j] = malloc(grid_size * (block_size + 1) * 4);
    args[j].true_roots = true_roots;
    args[j].colormap = colormap;
  }

  for(size_t n = 0; n < n_chunks; n++){
    if (n == n_chunks-1 && hasRemainder == 1) {
      block_size = block_size - 1;
    }

    // Allocate memory to the for_print we will write to

    for (t = 0, ix = 0; t < num_threads; t++, ix += block_size){
      args[t].ix = ix + n*num_threads*block_size;
      args[t].for_print_attr = &for_print_attr[n % 2][t][0];
      args[t].for_print_conv = &for_print_conv[n % 2][t][0];

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

    // If there has been writing, wait for that thread to finish before continuing
    if(n > 0){
      pthread_join(write_thread, NULL);
    }

    write_args.for_print_attr = for_print_attr[n % 2];
    write_args.for_print_conv = for_print_conv[n % 2];
    pthread_create(&write_thread, NULL, &write_method, (void *)&write_args);
  }

  pthread_join(write_thread, NULL);
  fclose(fp_attr);
  fclose(fp_conv);

  for(size_t j = 0; j < num_threads; j++){
    free(for_print_attr[0][j]);
    free(for_print_attr[1][j]);
    free(for_print_conv[0][j]);
    free(for_print_conv[1][j]);
  }

  free(for_print_attr[0]);
  free(for_print_attr[1]);
  free(for_print_conv[0]);
  free(for_print_conv[1]);
  free(for_print_attr);
  free(for_print_conv);
  free(args);

  for(int i = 0; i < d; i++ ){
    free(colormap[i]);
    free(true_roots[i]);
  }
  free(true_roots);
  free(colormap);
  return 0;
}
