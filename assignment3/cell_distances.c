#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

size_t n_threads;
#define max_pos (3465)
#define max_dist (3464)
#define fac (100)
#define n_coords (3)

void read_cells(){
  size_t lines=0,i,j;
  float dist;
  //char* filename = "cell_e5";
  //char* filename = "cell_e4";
  char* filename = "cells";

  FILE *fp = fopen(filename, "r");
  /*
  if (!strcmp(filename,"cell_e5")){
    lines = 100000;
  } else if(!strcmp(filename,"cell_e4")){
    lines = 10000;
  } else if(!strcmp(filename,"cells")){
    lines = 10;
  }
  */

  char ch = 0;
  while(!feof(fp))
  {
    ch = fgetc(fp);
    if(ch == '\n')
    {
      lines++;
    }
  }
  printf("%ld\n", lines);
  rewind(fp);

  size_t n = lines*n_coords;
  //size_t *count_array = calloc(max_pos, sizeof*count_array);
  //int *cell_array = calloc(n, sizeof *cell_array);
  float cell_array[n];
  size_t count_array[max_pos] ={0};

  float tmp[3];
  for(size_t i = 0; i<n; i+=n_coords){
    fscanf(fp, "%f %f %f", &tmp[0], &tmp[1], &tmp[2]);
    cell_array[i] = tmp[0]*fac;
    cell_array[i+1] = tmp[1]*fac;
    cell_array[i+2] = tmp[2]*fac;
  }

  fclose(fp);

  //for(size_t i = 0; i<lines*n_coords; i+=n_coords){
  //  printf("%d %d %d \n", cell_array[i], cell_array[i+1], cell_array[i+2]);
  //}

/*
http://stackoverflow.com/questions/20413995/reducing-on-array-in-openmp
*/
#pragma omp parallel shared(lines,cell_array)
{
  size_t count_array_private[max_pos] ={0};
  #pragma omp for private(i,j,dist) schedule(static,16)
  for(i = 0; i<n; i+=n_coords){
    for(j = i + n_coords; j<n; j+=n_coords){
      dist = sqrt((cell_array[i]-cell_array[j])*(cell_array[i]-cell_array[j])+
                  (cell_array[i+1]-cell_array[j+1])*(cell_array[i+1]-cell_array[j+1])+
                  (cell_array[i+2]-cell_array[j+2])*(cell_array[i+2]-cell_array[j+2]));
      count_array_private[(size_t)round(max_pos*dist/max_dist)]++;
    }
  }
      #pragma omp critical
    {
      for(i = 0; i<max_pos; i++){
        count_array[i] = count_array_private[i];
      }
    }
}
  for(i=0; i<max_pos;i++){
    if(count_array[i] != 0){
      printf("%1.2f %ld\n", 1.0f*max_dist*i/max_pos/fac, count_array[i]);
    }
  }

  //free(cell_array);
  //free(count_array);
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
  omp_set_num_threads(n_threads);
  read_cells();
  return 0;
}
