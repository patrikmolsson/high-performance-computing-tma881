#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define INFINITY 100000;
#define N_VERTICES 100;

int block_size;
int TAG_SEND_FROM_SLAVE = 2;

void slave(unsigned short *unvisited_set, unsigned short *distances);
void master(unsigned short *unvisited_set, unsigned short *distances);

int main(int argc, char** argv) {
  int n_vertices = N_VERTICES;

  unsigned short *unvisited_set; //Unvisited neighbours
  unvisited_set = (unsigned short *) malloc(n_vertices * sizeof *unvisited_set);

  for(int i = 0; i < n_vertices; i++ ){
    unvisited_set[i] = 1;
  }

  unsigned short *distances; //Unvisited neighbours
  distances = (unsigned short *) malloc(n_vertices * sizeof *distances);

  for(int i = 0; i < n_vertices; i++ ){
    distances[i] = 0;
  }

  // Initialize the MPI environment
  MPI_Init(NULL, NULL);

  // Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  block_size = n_vertices / (world_size - 1);

  // Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  printf("%d %d\n", world_rank, world_size);

  int finished = 0;
  while (finished < 10) {
    if (world_rank == 0) {
      master(unvisited_set, distances);
    } else {
      slave(unvisited_set, distances);
    }
    finished++;
  }

  // Get the name of the processor
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);
  // Finalize the MPI environment.
  MPI_Finalize();


  free(unvisited_set);
  free(distances);
}

void slave(unsigned short *unvisited_set, unsigned short *distances) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int startIndex = rank == 1 ? 0 : (rank - 1) * block_size - 1;
  int i;

  for (i = startIndex; i < block_size; i++) {
    if ((i == 0 && unvisited_set[0] == 1) || (unvisited_set[i - 1] == 0 && unvisited_set[i] == 1)) {
      distances[i] = 1;
      unvisited_set[i] = 0;
      break;
    }
  }

  int sendIndex[] = { i };
  MPI_Send(&sendIndex, 1, MPI_INT, 0, TAG_SEND_FROM_SLAVE, MPI_COMM_WORLD);
}

void master(unsigned short *unvisited_set, unsigned short *distances) {
  int sendIndex[1];
  MPI_Status status;
  MPI_Recv(&sendIndex, 1, MPI_INT, 1, TAG_SEND_FROM_SLAVE, MPI_COMM_WORLD, &status);
  printf("Receive from slave: %d\n", sendIndex[0]);
}
