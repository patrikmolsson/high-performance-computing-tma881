#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
//#define NUM_THREADS 5

void *PrintHello(void *threadid)
{
  long tid;
  tid = (long)threadid;
  printf("Hello World! It's me, thread #%ld!\n", tid);
  pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
  long num_threads;
  if(argc == 2){
    num_threads = (long)atoi(argv[1]);
    printf("Running with %ld threads in total\n",num_threads);
  } else {
    printf("Need to specify number of threads \n");
    exit(-1);
  }
  pthread_t threads[num_threads];
  int rc;
  long t;
  for(t=0; t<num_threads; t++){
     printf("In main: creating thread %ld\n", t);
     rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
     if (rc){
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
     }
  }

  /* Last thing that main() should do */
  pthread_exit(NULL);
  return 0;
}
