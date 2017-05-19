#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define INFINITY 100000;

typedef struct{
  unsigned short distance;
  unsigned long nachbar_id;
}nachbar_node;

const unsigned long  n_vertices = 1000; // max(n_vertices) = 1e5
const unsigned short degree = 10;

void read_adjacency(nachbar_node **nachbar_nodes){
  unsigned long i,j,lines=0; // 0 < i,j < n_vertices; lines dependent on number of connections. Need scan
  unsigned short dist; // 0 < dist < 100
  int fscan;
  //char* filename = "test_data/test_graph";
  
  char* filename = "test_data/graph_de1_ne3_we2";
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

// Method for every slave process. 
void slave(unsigned long current_id, nachbar_node **nachbar_nodes, unsigned long **tentative_distance){ 
  nachbar_node nachbar;
  for(unsigned short i = 0; i < degree; i++  ){
    nachbar = nachbar_nodes[current_id][i];
    if(tentative_distance[nachbar.nachbar_id][0] > tentative_distance[current_id][0] + nachbar.distance){
      tentative_distance[nachbar.nachbar_id][0] = tentative_distance[current_id][0] + nachbar.distance;
      tentative_distance[nachbar.nachbar_id][1] = current_id;
    }
    //printf("Visiting node %lu, checking node %lu: tent dist = %lu, \n",current_id,nachbar.nachbar_id,tentative_distance[nachbar.nachbar_id][0]);
  }
}

// Method for master process
void master(unsigned long source, unsigned long target){
  printf("\n Finding shortest distance from node %lu to node %lu \n \n",source,target);
  nachbar_node **nachbar_nodes; // Create adjacency matrix
  nachbar_nodes = malloc(n_vertices * sizeof *nachbar_nodes);
  
  for(int i = 0; i < n_vertices; i++ ){
    nachbar_nodes[i] = malloc(degree * sizeof *nachbar_nodes[i]);
  }

  read_adjacency(nachbar_nodes);

  unsigned long **tentative_distance; //Storing tentative
  tentative_distance = malloc(n_vertices * sizeof *tentative_distance);
 
  for(int i = 0; i < n_vertices; i++ ){
    tentative_distance[i] = malloc( 2 * sizeof *tentative_distance[i]);
    tentative_distance[i][0] = INFINITY;
  }
  
  tentative_distance[source][0] = 0;
  tentative_distance[source][1] = -1;

  unsigned short *unvisited_set; //Unvisited neighbours
  unvisited_set = malloc(n_vertices * sizeof *unvisited_set);
 
  for(int i = 0; i < n_vertices; i++ ){
    unvisited_set[i] = 1;
  }
  unsigned long current_id = source;
  unvisited_set[current_id] = 0;

  slave(current_id, nachbar_nodes, tentative_distance);
  while(current_id < n_vertices ){
    
    //printf("In while\n");
    current_id = n_vertices;
    unsigned long tmp_min = INFINITY; 
    for(unsigned long i = 0; i < n_vertices; i++ ){
      //printf("Checking node %lu \n", i);
      if(tentative_distance[i][0] < tmp_min && unvisited_set[i]> 0 ){
        tmp_min = tentative_distance[i][0];
        current_id = i;
      }
    }
    if(current_id == n_vertices)
      break;
    slave(current_id, nachbar_nodes, tentative_distance);
    unvisited_set[current_id] = 0;
  }
  
  current_id = target;
  // printf("current id: %lu, s:  %lu, t: %lu \n",current_id,source,target);
  printf("True path: %lu <-- ", target);
  while(current_id != source){
    printf("%lu",tentative_distance[current_id][1]);
    current_id = tentative_distance[current_id][1];
    if(current_id != source)
      printf(" <-- ");
    else
      printf("\n");
  }



  free(unvisited_set);

  for(int i = 0; i < n_vertices; i++ ){
    free(tentative_distance[i]);
  }
  free(tentative_distance);
  for(int i = 0; i < n_vertices; i++ ){
    free(nachbar_nodes[i]);
  }
  free(nachbar_nodes);
}

int main(int argc, char** argv) {
  unsigned long source = 9;
  unsigned long target = 82;
  // Initialize the MPI environment
  MPI_Init(NULL, NULL);

  // Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  // Get the name of the processor
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);
  master(source, target); 
  // Finalize the MPI environment.
  MPI_Finalize();
}
