#include <stdio.h>
#include <string.h>
#include <stdlib.h>

size_t num_threads;

void read_cells(){
  size_t lines = 0, n_coords = 3;
  char ch = 0;
  FILE *fp = fopen("cells", "r");
  while(!feof(fp))
  {
    ch = fgetc(fp);
    if(ch == '\n')
    {
      lines++;
    }
  }
  double cellArray[lines][n_coords];
  rewind(fp);
  for(size_t i = 0; i<lines; i++){
    fscanf(fp, "%lf %lf %lf", &cellArray[i][0], &cellArray[i][1], &cellArray[i][2]);
  }

  for(size_t i = 0; i<lines; i++){
    printf("%lf %lf %lf \n", cellArray[i][0], cellArray[i][1], cellArray[i][2]);
  }
  fclose(fp);
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