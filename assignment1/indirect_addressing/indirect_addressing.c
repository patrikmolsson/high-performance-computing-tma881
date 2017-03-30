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
    double time_tot = 0;
    clock_t begin, end;
    for(size_t i = 0; i < reps; ++i){        
        begin = clock();    
        func(y, x, p, n, a);    
        end = clock();
        time_tot += (end-begin);
    }
    return time_tot/CLOCKS_PER_SEC;
}

int main(){
    const size_t n = 1000000;
    size_t m = 1000;
    size_t *p; //Unsigned pga, we it know will be indices in p
    int *y, *x;
    const int a = 3;
    const size_t reps = 1000; 
    p = (size_t*) malloc(n*sizeof(size_t) );
    y = (int*) calloc(n,sizeof(int) );
    x = (int*) malloc(n*sizeof(int) );
     
    gen_indices(n, p);
    init_x(n,x);
    
    double ind_time = benchmark_function(y, x, p, n, a, *indirect_sum, reps);
    printf("Time indirect sum = %f \n",ind_time);
    
    double dir_time = benchmark_function(y, x, p, n, a, *direct_sum, reps);
    printf("Time indirect sum = %f \n",dir_time);
    

    free(p);
    free(x);
    free(y);
    
    return 0;
}

