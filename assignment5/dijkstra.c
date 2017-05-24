#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
  unsigned short distance;
  unsigned int nachbar_id;
} nachbar_node;

typedef struct{
  unsigned long status;
  unsigned long tentative_distance;
  unsigned long last_node;
} dijkstra_datum;

const long unsigned INFINITY = 1e9;

unsigned int n_vertices = 0;
unsigned short degree = 0;
unsigned int source = 5;
unsigned int target = 4;
unsigned long lines = 0;

const int VISITED = 0;
const int NOT_VISITED = 1;
const int REVISIT = 2;
const int BEEN_REVISITED = 3;

char filename[1000];
MPI_Datatype dType;
MPI_Op tentDistOp;

FILE *fp;

nachbar_node **nachbar_nodes;

void reduceDistance (void * in, void * inout, int * len, MPI_Datatype * mp) {
  dijkstra_datum * inLong = (dijkstra_datum *) in;
  dijkstra_datum * inoutLong = (dijkstra_datum *) inout;

  int inIsMin = 0;
  int isBothDistancesVisited = 0;
  for (int i = 0; i < *(len); i++) {
    inIsMin = inLong[i].tentative_distance < inoutLong[i].tentative_distance ? 1 : 0;

    if (inLong[i].tentative_distance < INFINITY
        && inoutLong[i].tentative_distance < INFINITY
        && inLong[i].tentative_distance != inoutLong[i].tentative_distance) {
      isBothDistancesVisited = 1;
    } else {
      isBothDistancesVisited = 0;
    }

    if (isBothDistancesVisited > 0) {
      inoutLong[i].status = REVISIT;
    } else if (inLong[i].status == BEEN_REVISITED && inoutLong[i].status == BEEN_REVISITED) {
      inoutLong[i].status = VISITED;
    } else if (inLong[i].status == BEEN_REVISITED ||  inoutLong[i].status == BEEN_REVISITED) {
      inoutLong[i].status = BEEN_REVISITED;
    } else if (inLong[i].status == REVISIT ||  inoutLong[i].status == REVISIT){
      inoutLong[i].status = REVISIT;
    } else {
      inoutLong[i].status = (inLong[i].status < inoutLong[i].status) ? inLong[i].status : inoutLong[i].status;
    }

    if (inIsMin > 0) {
      inoutLong[i].tentative_distance = inLong[i].tentative_distance;
      inoutLong[i].last_node = inLong[i].last_node;
    }
  }
}

void read_metadata() {
  unsigned long i,j;
  unsigned short dist;
  fp = fopen(filename, "r");

  // Count number of lines in file
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

  // Get number of degrees
  unsigned long prev_i = 0;
  while(fscanf(fp, "%lu %lu %hu", &i, &j, &dist)){
    if(i != prev_i){
      break;
    }

    degree++;
  }
  rewind(fp);

  n_vertices = lines / degree;
}

void read_adjacency(){
  unsigned long i, j, prev_i = 0;
  unsigned short dist;
  int fscan;

  unsigned short nachbar_count = 0;
  prev_i = -1;
  for(unsigned long l = 0; l < lines; l++){
    fscan = fscanf(fp, "%lu %lu %hu", &i, &j, &dist);
    if(i != prev_i)
      nachbar_count = 0;

    nachbar_nodes[i][nachbar_count].nachbar_id = j;
    nachbar_nodes[i][nachbar_count].distance = dist;
    nachbar_count++;
    prev_i = i;
  }

  fclose(fp);
}

void printPath(dijkstra_datum * dijkstra_data) {
  unsigned int current_id = target;
  unsigned long shortest_distance = dijkstra_data[current_id].tentative_distance;

  int n_steps = 0;
  while(current_id != source){
    n_steps++;
    current_id = dijkstra_data[current_id].last_node;
  }

  current_id = target;
  unsigned int steps[n_steps];
  for(int i = n_steps-1; i >= 0; i--){
    steps[i] = dijkstra_data[current_id].last_node;
    current_id = dijkstra_data[current_id].last_node;
  }

  printf("Shortest distance: %lu \nTrue and actual path: ", shortest_distance);
  for(int i = 0; i < n_steps; i++ ){
    printf("%u -> ",steps[i]);
  }
  printf("%u\n",target);
}

// Method for every slave process.
void dijkstra(unsigned int current_id, nachbar_node **nachbar_nodes, dijkstra_datum dijkstra_data[]){
  nachbar_node nachbar;
  for(unsigned short i = 0; i < degree; i++  ){
    nachbar = nachbar_nodes[current_id][i];
    if(dijkstra_data[nachbar.nachbar_id].tentative_distance > dijkstra_data[current_id].tentative_distance + nachbar.distance){
      dijkstra_data[nachbar.nachbar_id].tentative_distance = dijkstra_data[current_id].tentative_distance + nachbar.distance;
      dijkstra_data[nachbar.nachbar_id].last_node = current_id;
    }
  }
}

// Method for master process
void startMethod(){
  dijkstra_datum dijkstra_data[n_vertices];

  for(unsigned int i = 0; i < n_vertices; i++) {
    dijkstra_data[i].tentative_distance = INFINITY;
    dijkstra_data[i].last_node = n_vertices;
    dijkstra_data[i].status = NOT_VISITED;
  }

  // Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  unsigned int current_id = source;
  if (world_rank == 0) {
    dijkstra_data[current_id].tentative_distance = 0;
    dijkstra(current_id, nachbar_nodes, dijkstra_data);
    dijkstra_data[current_id].status = 0;
  }

  int block_size = n_vertices / world_size;

  while(1){
    MPI_Allreduce(MPI_IN_PLACE, &dijkstra_data, n_vertices, dType, tentDistOp, MPI_COMM_WORLD);

    int hasUnfinishedNodes = 0;
    for(unsigned int i = 0; i < n_vertices; i++) {
      if (dijkstra_data[i].status > 0) {
        hasUnfinishedNodes = 1;
        break;
      }
    }

    if (hasUnfinishedNodes == 0) {
      break;
    }

    current_id = n_vertices;
    unsigned long tmp_min = INFINITY;

    for(int i = world_rank * block_size; i < (world_rank + 1) * block_size; i++) {
      if(dijkstra_data[i].tentative_distance < tmp_min &&
          (dijkstra_data[i].status == NOT_VISITED || dijkstra_data[i].status == REVISIT)) {
        tmp_min = dijkstra_data[i].tentative_distance;
        current_id = i;
      }
    }
    if (current_id != n_vertices) {
      dijkstra(current_id, nachbar_nodes, dijkstra_data);
      dijkstra_data[current_id].status = dijkstra_data[current_id].status == REVISIT ? BEEN_REVISITED : VISITED;
    }
  }

  // PRINT RESULTS
  if (world_rank == 0)
    printPath(dijkstra_data);
}

int main(int argc, char* argv[]) {
  // Initialize the MPI environment
  MPI_Init(&argc, &argv);

  for (int i = 1; i < argc; i++) {
    if (i == 1)
      source = strtol(argv[i], NULL, 10);
    else if (i == 2)
      target = strtol(argv[i], NULL, 10);
    else if (i == 3) {
      strcpy(filename, argv[i]);
    }
  }

  // Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  if (world_rank == 0)
    read_metadata();

  MPI_Bcast(&n_vertices, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
  MPI_Bcast(&degree, 1, MPI_UNSIGNED_SHORT, 0, MPI_COMM_WORLD);

  nachbar_nodes = malloc(n_vertices * sizeof *nachbar_nodes);

  for(unsigned int i = 0; i < n_vertices; i++ ){
    nachbar_nodes[i] = malloc(degree * sizeof *nachbar_nodes[i]);
  }

  if (world_rank == 0)
    read_adjacency();


  int blocklengths[] = {1, 1};
  MPI_Datatype types[2] = {MPI_UNSIGNED_SHORT, MPI_UNSIGNED};
  MPI_Datatype mpi_nn;
  MPI_Aint offsets[2];

  offsets[0] = offsetof(nachbar_node, distance);
  offsets[1] = offsetof(nachbar_node, nachbar_id);

  MPI_Type_create_struct(2, blocklengths, offsets, types, &mpi_nn);
  MPI_Type_commit(&mpi_nn);

  MPI_Bcast(&nachbar_nodes[0][0], n_vertices * degree, mpi_nn, 0, MPI_COMM_WORLD);

  MPI_Op_create(reduceDistance, 1, &tentDistOp);

  MPI_Type_contiguous(3, MPI_UNSIGNED_LONG, &dType);
  MPI_Type_commit(&dType);

  // Initialize our main solver
  startMethod();

  // Finalize the MPI environment.
  MPI_Type_free(&mpi_nn);
  MPI_Type_free(&dType);
  MPI_Finalize();

  for(unsigned int i = 0; i < n_vertices; i++ ){
    free(nachbar_nodes[i]);
  }
  free(nachbar_nodes);

  return 0;
}
