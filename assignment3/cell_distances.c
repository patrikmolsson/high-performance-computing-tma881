#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
//#include "emmintrin.h"
#include <xmmintrin.h>
#include <pmmintrin.h>
#include <sse_mathfun.h>

size_t n_threads;
static const size_t max_pos = 3465;
static const float max_dist = 34.64f;
static const size_t fac = 100;

float hsum_ps_sse3(__m128 v) {
    __m128 shuf = _mm_movehdup_ps(v);        // broadcast elements 3,1 to 2,0
    __m128 sums = _mm_add_ps(v, shuf);
    shuf        = _mm_movehl_ps(shuf, sums); // high half -> low half
    sums        = _mm_add_ss(sums, shuf);
    return        _mm_cvtss_f32(sums);
}

void read_cells(){
  size_t lines = 0, n_coords = 4,i,j;
  float dist;
  //char* filename = "cell_e5";
  //char* filename = "cell_e4";
  char* filename = "cells";
  size_t *count_array;
  float *cell_array;

  FILE *fp = fopen(filename, "r");
  if (!strcmp(filename,"cell_e5")){
    lines = 100000;
  } else if(!strcmp(filename,"cell_e4")){
    lines = 10000;
  } else if(!strcmp(filename,"cells")){
    lines = 10;
  }

  /*char ch = 0;
  while(!feof(fp))
  {
    ch = fgetc(fp);
    if(ch == '\n')
    {
      lines++;
    }
  }
  rewind(fp);*/
  count_array = calloc(max_pos, sizeof*count_array);
  cell_array = calloc(lines * n_coords, sizeof *cell_array);

  for(size_t i = 0; i<lines*n_coords; i+=n_coords){
    fscanf(fp, "%f %f %f", &cell_array[i], &cell_array[i+1], &cell_array[i+2]);
    cell_array[i+3] = 0;
  }

  fclose(fp);

  //for(size_t i = 0; i<lines*n_coords; i+=n_coords){
  //  printf("%f %f %f \n", cell_array[i], cell_array[i+1], cell_array[i+2]);
  //}
  __m128 va,vb,diff_ab,mul_ab;
  #pragma omp parallel for private(i,j,dist,va,vb,diff_ab,mul_ab) shared(cell_array,count_array) num_threads(n_threads)
  for(i = 0; i<lines*n_coords; i+=n_coords){
    for(j = i + n_coords; j<lines*n_coords; j+=n_coords){
      va = _mm_load_ps(&cell_array[i]);
      vb = _mm_load_ps(&cell_array[j]);
      diff_ab = _mm_sub_ps(va, vb);
      mul_ab = exp_ps(2*log_ps(diff_ab));
      dist = hsum_ps_sse3(mul_ab);
      dist = round(sqrt(dist)*fac)/fac;
      count_array[(size_t)(dist/max_dist*max_pos)]++;
      /*dist = sqrt((cell_array[i]-cell_array[j])*(cell_array[i]-cell_array[j])+
                  (cell_array[i+1]-cell_array[j+1])*(cell_array[i+1]-cell_array[j+1])+
                  (cell_array[i+2]-cell_array[j+2])*(cell_array[i+2]-cell_array[j+2]));
      dist = round(dist*fac)/fac;
      count_array[(size_t)(dist/max_dist*max_pos)]++;*/
    }
  }

  for(size_t i=0; i<max_pos;i++){
    if(count_array[i] != 0){
      printf("%1.2f %ld\n", max_dist*i/max_pos, count_array[i]);
    }
  }

  free(cell_array);
  free(count_array);
}

int main(int argc, char *argv[]){
  char arg[10];

  for (size_t i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      memset(arg, 0, sizeof(arg));
      memcpy(arg, &argv[i][2], strlen(argv[i]) - 2);
      if (argv[i][1] == 't') {
        n_threads = (size_t) strtol(arg, NULL, 10);
      }

    }
  }

  read_cells();
  return 0;
}
