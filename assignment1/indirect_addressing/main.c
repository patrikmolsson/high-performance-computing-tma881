#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>

typedef void (*FUNC_PTR)( int*, int*, size_t*, size_t, int );

/*double print_current_time_with_ms (void){
    long ms; // Milliseconds
    time_t s;  // Seconds
    struct timespec spec;
    clock_gettime(CLOCK_MONOTONIC, &spec);
    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6);
    long ns = spec.tv_nsec;
    printf( "Time ns: %ld\n ",ns );
    printf("Current time: %"PRIdMAX".%05ld seconds since the Epoch\n",(intmax_t)s, ms);
}*/

static double timespec_to_seconds (struct timespec* ts){
	return (double)ts -> tv_sec + (double)ts -> tv_nsec / 1000000000.0;
}
  

void gen_indices(const size_t n, size_t *p ){
    for( size_t ix = 0; ix < n; ++ix ){
        p[ix] = ix;
    }
}

void init_x(const size_t n, int *x ){
    for( size_t ix = 0; ix < n; ++ix ){
        x[ix] = rand() % 50;
    }
}

void indirect_sum( int *y, int *x, size_t *p, const size_t n, const int a ){
    size_t jx;
    for ( size_t kx = 0; kx < n; ++kx){
        jx = p[kx];
        y[jx] += a * x[jx];
    }
}

void direct_sum( int *y, int *x, size_t *p, const size_t n, const int a ){
    for ( size_t kx = 0; kx < n; ++kx){
        y[kx] += a * x[kx];
    }
}

double benchmark_function(int *y, int *x, size_t *p, const size_t n, const int a, FUNC_PTR
func, const size_t reps){
	struct timespec start;
	struct timespec end;
	double elapsed_seconds = 0;
	
    	for(size_t i = 0; i < reps; ++i){        
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        	func(y, x, p, n, a);    
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    		elapsed_seconds += timespec_to_seconds(&end) - timespec_to_seconds(&start);
    		}
	return elapsed_seconds;
}

int main(){
    const size_t n = 1000000;
    size_t m = 1000;
    size_t *p; //Unsigned pga, we it know will be indices in p
    int *y, *x;
    const int a = 3; // Not changing the coefficient
    const size_t reps = 1000; // Number of executions for timing.
    p = (size_t*) malloc(n*sizeof(size_t) );
    y = (int*) calloc(n,sizeof(int) ); // y zero vector initially
    x = (int*) malloc(n*sizeof(int) );
     
    gen_indices(n, p); // Fill the vector p with indices
    init_x(n,x); // Fill the addition vector
    
    double ind_time = benchmark_function(y, x, p, n, a, *indirect_sum, reps);
    printf("Time indirect sum = %.9f \n",ind_time);
        
    double dir_time = benchmark_function(y, x, p, n, a, *direct_sum, reps);
    printf("Time direct sum = %.9f \n",dir_time);
    

    free(p);
    free(x);
    free(y);
    
    return 0;
}

