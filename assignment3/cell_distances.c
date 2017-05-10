#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

size_t n_threads;
#define max_pos (3466)
#define fac (100)
#define n_coords (3)

void read_cells(){
  unsigned long i,j,lines=0;
  unsigned short dist;
  int fscan;
  //char* filename = "cell_e5";
  //char* filename = "cell_e4";
  char* filename = "cells";

  FILE *fp = fopen(filename, "r");

  char ch = 0;
  while(!feof(fp))
  {
    ch = fgetc(fp);
    if(ch == '\n')
    {
      lines++;
    }
  }
  rewind(fp);

  float cell_array[lines][3];
  unsigned long count_array[max_pos];
  memset(count_array, 0, max_pos*sizeof(unsigned long));

  for(i = 0; i<lines; i++){
    fscan = fscanf(fp, "%f %f %f", &cell_array[i][0], &cell_array[i][1], &cell_array[i][2]);
  }

  fclose(fp);

  #pragma omp parallel shared(lines,cell_array)
  {
    unsigned long count_array_private[max_pos];
    memset(count_array_private, 0, max_pos*sizeof(unsigned long));
    #pragma omp for private(i,j,dist) schedule(static,32)
    for(i = 0; i<lines; i++){
      for(j = i + 1; j<lines; j++){
        dist = roundf(sqrtf((cell_array[i][0]-cell_array[j][0])*(cell_array[i][0]-cell_array[j][0])+
                            (cell_array[i][1]-cell_array[j][1])*(cell_array[i][1]-cell_array[j][1])+
                            (cell_array[i][2]-cell_array[j][2])*(cell_array[i][2]-cell_array[j][2]))*fac);
        count_array_private[dist]++;
      }
    }
    #pragma omp critical
    {
      for(i = 0; i<max_pos; i++){
        count_array[i] += count_array_private[i];
      }
    }
  }

  for(i=0; i<max_pos;i++){
    printf("%05.2f %ld\n", 1.0f*i/fac, count_array[i]);
  }
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
