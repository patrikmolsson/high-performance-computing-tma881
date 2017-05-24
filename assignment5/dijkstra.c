#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define INFINITY 100000;

const unsigned short TAG_SEND_FROM_SLAVE = 2;

typedef struct{
  unsigned short distance;
  unsigned int nachbar_id;
} nachbar_node;

const unsigned int  n_vertices = 8; // max(n_vertices) = 1e5
const unsigned short degree = 3;

void read_adjacency(nachbar_node **nachbar_nodes){
  unsigned long i,j,lines=0; // 0 < i,j < n_vertices; lines dependent on number of connections. Need scan
  unsigned short dist; // 0 < dist < 100
  int fscan;
  char* filename = "test_data/test_graph";
  //char* filename = "test_data/graph_de2_ne4_we2";
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
  ////printf("lines = %lu \n", lines); 
  //memset(count_array, 0, max_pos*sizeof(unsigned long));
  unsigned short nachbar_count = 0;
  unsigned long prev_i = -1;
  for(unsigned long l = 0; l<lines; l++){
    fscan = fscanf(fp, "%lu %lu %hu", &i, &j, &dist);
    if(i != prev_i ){
      nachbar_count = 0;
    }
    nachbar_nodes[i][nachbar_count].nachbar_id = j;
    nachbar_nodes[i][nachbar_count].distance = dist;
    nachbar_count++;
    prev_i = i;
  }

  fclose(fp);
}

void printPath(int target, int source, unsigned long *tentative_distance) {
  int current_id = target;
  unsigned long shortest_distance = tentative_distance[current_id];
  // printf("current id: %lu, s:  %lu, t: %lu \n",current_id,source,target);
  int  n_steps = 0;
  while(current_id != source){
    n_steps ++;
    current_id = tentative_distance[current_id + n_vertices];
  }

  current_id = target;
  unsigned int steps[n_steps];
  for(int i = n_steps-1; i >= 0; i--){
    steps[i] = tentative_distance[current_id + n_vertices];
    current_id = tentative_distance[current_id + n_vertices];
  }
  printf("Shortest distance: %lu \nTrue and actual path: ",shortest_distance);//
  for(int i = 0; i < n_steps; i++ ){
    printf("%u -> ",steps[i]);
  }
  printf("%u\n",target);
}

// Method for every slave process.
void dijkstra(unsigned int current_id, nachbar_node **nachbar_nodes, unsigned long *tentative_distance){
  nachbar_node nachbar;
  for(unsigned short i = 0; i < degree; i++  ){
    nachbar = nachbar_nodes[current_id][i];
    if(tentative_distance[nachbar.nachbar_id] > tentative_distance[current_id] + nachbar.distance){
      tentative_distance[nachbar.nachbar_id] = tentative_distance[current_id] + nachbar.distance;
      tentative_distance[nachbar.nachbar_id + n_vertices] = current_id;
    }
    //printf("Visiting node %lu, checking node %lu: tent dist = %lu, \n",current_id,nachbar.nachbar_id,tentative_distance[nachbar.nachbar_id][0]);
  }
}

// Method for master process
void startMethod(unsigned int source, unsigned int target){
  // START INIT VARIABLES
  printf("\nFinding shortest distance from node %u to node %u \n \n",source,target);
  nachbar_node **nachbar_nodes; // Create adjacency matrix
  nachbar_nodes = malloc(n_vertices * sizeof *nachbar_nodes);

  for(int i = 0; i < n_vertices; i++ ){
    nachbar_nodes[i] = malloc(degree * sizeof *nachbar_nodes[i]);
  }

  read_adjacency(nachbar_nodes);

  unsigned long *tentative_distance; //Storing tentative
  tentative_distance = malloc(2 * n_vertices * sizeof *tentative_distance);
  // First half distances
  // Second half source

  for(unsigned int i = 0; i < n_vertices; i++ ){
    tentative_distance[i] = INFINITY;
    tentative_distance[i + n_vertices] = 0;
  }

  tentative_distance[source] = 0;
  tentative_distance[source + n_vertices] = -1;

  short *unvisited_set; //Unvisited neighbours
  unvisited_set = (short *) calloc(n_vertices, sizeof *unvisited_set);

  for(unsigned int i = 0; i < n_vertices; i++ ){
    unvisited_set[i] = 1;
  }
  // STOP INIT VARIABLES

  // Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  unsigned int current_id = source;
  if (world_rank == 0) {
    dijkstra(current_id, nachbar_nodes, tentative_distance);
    unvisited_set[current_id] = 0;
  }

  int block_size = n_vertices / world_size;

  while(1){
    // BROADCAST FROM MASTER IF BREAK
    //
    //MPI_Bcast(unvisited_set, n_vertices, MPI_UNSIGNED_SHORT, 0, MPI_COMM_WORLD);
  //  MPI_Bcast(tentative_distance, 2 * n_vertices, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    for(unsigned int i = 0; i < n_vertices; i++ ){
      printf("Before: Index %d, Value %d, Rank %d\n", i, unvisited_set[i], world_rank);
    }

    //if (world_rank == 0)
    //  MPI_Reduce(MPI_IN_PLACE, &unvisited_set, n_vertices, MPI_SHORT, MPI_MIN, 0, MPI_COMM_WORLD);
    //else
    //  MPI_Reduce(&unvisited_set, &unvisited_set, n_vertices, MPI_SHORT, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Allreduce(MPI_IN_PLACE, &unvisited_set, n_vertices, MPI_SHORT, MPI_MIN, MPI_COMM_WORLD);
    //MPI_Allreduce(MPI_IN_PLACE, &tentative_distance, 2 * n_vertices, MPI_UNSIGNED_LONG, MPI_MIN, MPI_COMM_WORLD);

    printf("All reduce finished %d\n", world_rank);

    int sum = 0;
    // for(int i = world_rank * block_size; i < (world_rank + 1) * block_size; i++ ){
    for(unsigned int i = 0; i < n_vertices; i++ ){
      printf("After: Index %d, Value %d, Rank %d\n", i, unvisited_set[i], world_rank);
      sum += unvisited_set[i];
    }
    printf("Sum calculated %d\n", sum);
    if (sum == 0)
      break;


    /*
  current_id = n_vertices;
  unsigned long tmp_min = INFINITY;
  for(unsigned int i = 0; i < n_vertices; i++ ){
    if(tentative_distance[i] < tmp_min){
      printf("Rank %d, Node %i, dist %lu\n", world_rank, i, tentative_distance[i]);
    }
  }
  */
    current_id = n_vertices;
    unsigned long tmp_min = INFINITY;

    for(int i = world_rank * block_size; i < (world_rank + 1) * block_size; i++ ){
      printf("Trying to find new index %d %d\n", i, world_rank);
      if(tentative_distance[i] < tmp_min && unvisited_set[i] > 0 ){
        tmp_min = tentative_distance[i];
        current_id = i;
      }
    }
    if (current_id != n_vertices) {
      dijkstra(current_id, nachbar_nodes, tentative_distance);
      unvisited_set[current_id] = 0;
    }
  }

  // PRINT RESULTS
  if (world_rank == 0)
    printPath(target, source, tentative_distance);

  free(unvisited_set);

  free(tentative_distance);
  for(unsigned int i = 0; i < n_vertices; i++ ){
    free(nachbar_nodes[i]);
  }
  free(nachbar_nodes);
}

int main(int argc, char** argv) {
  unsigned int source = 5;
  unsigned int target = 4;
  // Initialize the MPI environment
  MPI_Init(&argc, &argv);

  //printf("Proc ID: %d", world_rank);
  // Get the name of the processor
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);
  startMethod(source, target); 
  // Finalize the MPI environment.
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
