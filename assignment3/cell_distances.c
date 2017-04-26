#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

size_t num_threads;
static const size_t max_pos = 3465;
//static const double max_dist = 34.641016;//...
static const double max_dist = 34.64f;

void read_cells(){
  size_t lines = 0, n_coords = 3, fac = 100;
  char ch = 0;
  //FILE *fp = fopen("cell_e4", "r");
  FILE *fp = fopen("cell_e4", "r");
  float dist;
  while(!feof(fp))
  {
    ch = fgetc(fp);
    if(ch == '\n')
    {
      lines++;
    }
  }
  //double cell_array[lines][n_coords]; // double?
  size_t *count_array;
  float **cell_array;
  cell_array = malloc(lines * sizeof *cell_array);
  for(size_t k = 0; k < lines; k++){
    cell_array[k] = malloc(n_coords *sizeof *cell_array[k]);
  }
  count_array = calloc(max_pos,sizeof*count_array);

  rewind(fp);
  for(size_t i = 0; i<lines; i++){
    fscanf(fp, "%f %f %f", &cell_array[i][0], &cell_array[i][1], &cell_array[i][2]);
  }

  //for(size_t i = 0; i<lines; i++){
  //  printf("%f %f %f \n", cell_array[i][0], cell_array[i][1], cell_array[i][2]);
  //}
  fclose(fp);
  size_t i,j;
  #pragma omp parallel for private(i,j,dist) shared(cell_array,count_array,fac) num_threads(1)
  for(i = 0; i<lines; i++){
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
  free(count_array);
  for(size_t k = 0; k < lines; k++){
    free(cell_array[k]);
  }
  free(cell_array);
}

int main(int argc, char *argv[]){
  char arg[10];

  for (size_t i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      memset(arg, 0, sizeof(arg));
      memcpy(arg, &argv[i][2], strlen(argv[i]) - 2);
      if (argv[i][1] == 't') {
        num_threads = (size_t) strtol(arg, NULL, 10);
      }

    }
  }

  read_cells();
  return 0;
}
