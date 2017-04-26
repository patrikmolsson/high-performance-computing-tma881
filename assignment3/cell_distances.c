#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

size_t n_threads;
static const size_t max_pos = 3465;
static const double max_dist = 34.64f;
static const size_t fac = 100;

void read_cells(){
  size_t lines = 0, n_coords = 3,i,j;
  float dist;
  //char* filename = "cell_e5";
  char* filename = "cell_e4";
  //char* filename = "cells";
  size_t *count_array;
  float **cell_array;

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
  count_array = calloc(max_pos,sizeof*count_array);
  cell_array = malloc(lines * sizeof *cell_array);
  for(size_t k = 0; k < lines; k++){
    cell_array[k] = malloc(n_coords *sizeof *cell_array[k]);
  }

  for(size_t i = 0; i<lines; i++){
    fscanf(fp, "%f %f %f", &cell_array[i][0], &cell_array[i][1], &cell_array[i][2]);
  }

  fclose(fp);

  //for(size_t i = 0; i<lines; i++){
  //  printf("%f %f %f \n", cell_array[i][0], cell_array[i][1], cell_array[i][2]);
  //}
  #pragma omp parallel for private(i,j,dist) shared(cell_array,count_array) num_threads(n_threads)
  //omp_set_nested(1);
  //#pragma omp parallel for private(i) shared(cell_array,count_array) num_threads(n_threads)
  for(i = 0; i<lines; i++){
    //#pragma omp parallel for private(j,dist) shared(cell_array,count_array) num_threads(2)
    for(j = i + 1; j<lines; j++){
      dist = sqrt((cell_array[i][0]-cell_array[j][0])*(cell_array[i][0]-cell_array[j][0])+
                  (cell_array[i][1]-cell_array[j][1])*(cell_array[i][1]-cell_array[j][1])+
                  (cell_array[i][2]-cell_array[j][2])*(cell_array[i][2]-cell_array[j][2]));
      dist = roundf(dist*fac)/fac;
      count_array[(size_t)(dist/max_dist*max_pos)]++;
    }
  }

  for(size_t i=0; i<max_pos;i++){
    if(count_array[i] != 0){
      printf("%1.2f %ld\n", max_dist*i/max_pos, count_array[i]);
    }
  }

  for(size_t k = 0; k < lines; k++){
    free(cell_array[k]);
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
