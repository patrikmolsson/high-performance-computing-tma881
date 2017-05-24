#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define INFINITY 100000;

const unsigned short TAG_SEND_FROM_SLAVE = 2;

typedef struct{
  unsigned short distance;
  unsigned int nachbar_id;
} nachbar_node;

typedef struct{
  unsigned long unvisited;
  unsigned long tentative_distance;
  unsigned long last_node;
} dijkstra_datum;

const unsigned int  n_vertices = 8; // max(n_vertices) = 1e5
const unsigned short degree = 3;

void reduceDistance (void * in, void * inout, int * len, MPI_Datatype * mp) {
  dijkstra_datum * inLong = (dijkstra_datum *) in;
  dijkstra_datum * inoutLong = (dijkstra_datum *) inout;

  int inIsMin = 0;
  for (int i = 0; i < *(len); i++) {
    inIsMin = inLong[i].tentative_distance < inoutLong[i].tentative_distance ? 1 : 0;

    if (inLong[i].unvisited == 2 ||  inoutLong[i].unvisited == 2){
      inoutLong[i].unvisited = 1;
    } else {
      inoutLong[i].unvisited = (inLong[i].unvisited < inoutLong[i].unvisited) ? inLong[i].unvisited : inoutLong[i].unvisited;
    }

    printf("I: %d, In %d %d, \t\tinout %d %d\n", i, inLong[i].tentative_distance, inLong[i].tentative_distance, inoutLong[i].tentative_distance, inoutLong[i].tentative_distance);

    if (inIsMin > 0) {
      inoutLong[i].tentative_distance = inLong[i].tentative_distance;
      inoutLong[i].last_node = inLong[i].last_node;
    }
  }
}

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

void printPath(int target, int source, dijkstra_datum * dijkstra_data) {
  printf("Into printpath");
  int current_id = target;
  unsigned long shortest_distance = dijkstra_data[current_id].tentative_distance;
  // printf("current id: %lu, s:  %lu, t: %lu \n",current_id,source,target);
  int  n_steps = 0;
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
  printf("Shortest distance: %lu \nTrue and actual path: ",shortest_distance);//
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
      dijkstra_data[nachbar.nachbar_id].unvisited = 2;
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

  dijkstra_datum dijkstra_data[n_vertices];

  for(int i = 0; i < n_vertices; i++ ){
    dijkstra_data[i].tentative_distance = INFINITY;
    dijkstra_data[i].last_node = n_vertices;
    dijkstra_data[i].unvisited = 1;
  }

  MPI_Op tentDistOp;
  MPI_Op_create(reduceDistance, 1, &tentDistOp);

  MPI_Datatype dType;
  MPI_Type_contiguous(3, MPI_UNSIGNED_LONG, &dType);
  MPI_Type_commit(&dType);
  // STOP INIT VARIABLES

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
    dijkstra_data[current_id].unvisited = 0;
  }

  int block_size = n_vertices / world_size;
  int loop = 1;

  while(loop == 1){
    // BROADCAST FROM MASTER IF BREAK
    //
    //MPI_Bcast(unvisited_set, n_vertices, MPI_UNSIGNED_SHORT, 0, MPI_COMM_WORLD);
  //  MPI_Bcast(tentative_distance, 2 * n_vertices, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    //if (world_rank == 0)
    //  MPI_Reduce(MPI_IN_PLACE, &unvisited_set, n_vertices, MPI_SHORT, MPI_MIN, 0, MPI_COMM_WORLD);
    //else
    //  MPI_Reduce(&unvisited_set, &unvisited_set, n_vertices, MPI_SHORT, MPI_MIN, 0, MPI_COMM_WORLD);
    printf("Waiting for reduce %d\n", world_rank);
    for(unsigned int i = 0; i < n_vertices; i++ ){
      printf("Status: %lu %lu %lu\n", dijkstra_data[i].tentative_distance, dijkstra_data[i].last_node, dijkstra_data[i].unvisited);
    }
    //MPI_Allreduce(MPI_IN_PLACE, &unvisited_set, n_vertices, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    MPI_Allreduce(MPI_IN_PLACE, &dijkstra_data, n_vertices, dType, tentDistOp, MPI_COMM_WORLD);

    printf("All reduce finished %d\n", world_rank);

    int sum = 0;
    // for(int i = world_rank * block_size; i < (world_rank + 1) * block_size; i++ ){
    for(unsigned int i = 0; i < n_vertices; i++ ){
      sum += dijkstra_data[i].unvisited;
    }
    printf("Sum calculated %d, rank %d\n", sum, world_rank);
    if (sum == 0) {
      printf("Breaking loop %d\n", world_rank);
      MPI_Barrier(MPI_COMM_WORLD);
      break;
    }


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
      if(dijkstra_data[i].tentative_distance < tmp_min && dijkstra_data[i].unvisited > 0 ){
        tmp_min = dijkstra_data[i].tentative_distance;
        current_id = i;
      }
    }
    if (current_id != n_vertices) {
      printf("Visiting %d\n", current_id);
      dijkstra(current_id, nachbar_nodes, dijkstra_data);
      dijkstra_data[current_id].unvisited = 0;
    }
  }

  printf("Exiting while loop %d\n", world_rank);
  // PRINT RESULTS
  if (world_rank == 0) {
    printf("Starting printpath");
    printPath(target, source, dijkstra_data);
  }



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
  MPI_Finalize();

  return 0;
}
