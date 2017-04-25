#include "math.h"
#include <iostream>
#include <string>
#include <map>
//#include <unordered_map>
#include <vector>

size_t num_threads;

void read_cells(){
  size_t lines = 0, n_coords = 3, fac = 100; // fac sets how many decimal of prec
  char ch = 0;
  //FILE *fp = fopen("cells", "r");
  FILE *fp = fopen("cell_e5", "r");
  double dist;
  double **cell_array;
  //std::unordered_map<double, int> dists;
  std::map<double, int> dists;
  while(!feof(fp))
  {
    ch = fgetc(fp);
    if(ch == '\n')
    {
      lines++;
    }
  }
  //double cell_array[lines][n_coords];
  cell_array = (double **) malloc(lines * sizeof *cell_array);
  for(size_t i = 0; i < lines; i++ ){
    cell_array[i] = (double *) malloc(n_coords * sizeof *cell_array[i]);
  }

  rewind(fp);

  for(size_t i = 0; i<lines; i++){
    fscanf(fp, "%lf %lf %lf", &cell_array[i][0], &cell_array[i][1], &cell_array[i][2]);
  }
  fclose(fp);

  for(size_t i = 0; i<lines/100; i++){
    for(size_t j = i + 1; j< lines; j++){
      dist = sqrt(pow(cell_array[i][0]-cell_array[j][0],2)+
                  pow(cell_array[i][1]-cell_array[j][1],2)+
                  pow(cell_array[i][2]-cell_array[j][2],2));
      dist = round(dist*fac)/fac;
      dists[dist]++;
    }
  }

  //for(auto kv : dists){
    //std::cout.precision(2);
    //std::cout << std::fixed << kv.first << " " << kv.second << std::endl;
  //}
  for(size_t i = 0; i < lines; i++ ){
    free(cell_array[i]);
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