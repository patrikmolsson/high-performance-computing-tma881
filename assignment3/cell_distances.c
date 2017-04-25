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
  FILE *fp = fopen("cell_e5", "r");
  double dist;
  while(!feof(fp))
  {
    ch = fgetc(fp);
    if(ch == '\n')
    {
      lines++;
    }
  }
  double cell_array[lines][n_coords]; // double?
  size_t *count_array;
  count_array = calloc(max_pos,sizeof*count_array);

  rewind(fp);
  for(size_t i = 0; i<lines; i++){
    fscanf(fp, "%lf %lf %lf", &cell_array[i][0], &cell_array[i][1], &cell_array[i][2]);
  }

  for(size_t i = 0; i<lines; i++){
    //printf("%lf %lf %lf \n", cell_array[i][0], cell_array[i][1], cell_array[i][2]);
  }
  fclose(fp);
  #pragma omp parallel for collapse(2)
  for(size_t i = 0; i<lines; i++){
    for(size_t j = i + 1; j<lines; j++){
      dist = sqrt((cell_array[i][0]-cell_array[j][0])*
                  (cell_array[i][0]-cell_array[j][0])+
                  (cell_array[i][1]-cell_array[j][1])*
                  (cell_array[i][1]-cell_array[j][1])+
                  (cell_array[i][2]-cell_array[j][2])*
                  (cell_array[i][2]-cell_array[j][2]));
      dist = round(dist*fac)/fac;
      //printf("dist %lf\n",dist);
      count_array[(size_t)(dist/max_dist*max_pos)]++;
      //printf("dist %f ind %d\n",dist, (size_t)(dist/max_dist*max_pos));
    }
  }
  for(size_t i=0; i<max_pos;i++){
    if(count_array[i] != 0){
      printf("%1.2f %ld\n", max_dist*i/max_pos, count_array[i]);
    }
  }
  free(count_array);
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