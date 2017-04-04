#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>

typedef struct{
	double complex root;
	int iter_conv;
	int type_conv;
} newton_res;

static const double TOL_CONV = 1e6;
static const double TOL_DIV = 10e10;

double complex newton_iterate(double complex x_0, size_t d){

	double complex x_1 = x_0 -( cpow(x_0,d) - 1.0) / ( (double) d * cpow(x_0,d-1) );
	
	return x_1;
}

void find_true_roots(size_t d, complex double *true_root){
	/* One class of polynom: x^d - 1 = 0; => (a) one root always Re(root) = 1; (b) complex roots conjugate  */
	true_root[0] = 1.0;
	for(size_t i=1; i<d; i++ ){
		double arg = i*2*M_PI/d;
		true_root[i] = cos(arg) + sin(arg)*I; 
	}
}


void newton_method(newton_res *result, const complex double x_init, const size_t d){
	int conv = 0;
	complex double x_0 = x_init;
	complex double x_1;
	complex double true_root[d];
	find_true_roots(d, true_root);
	size_t iter = 0;

	while(conv != 1 && cabs(x_0) > TOL_CONV && creal(x_0) < TOL_DIV && cimag(x_0 ) < TOL_DIV && iter < 1e3 ){
		x_1 = newton_iterate(x_0, d);
		for(size_t i=0; i<d;i++){
			if (cabs(x_1-true_root[i]) < TOL_CONV){
				conv = 1;
			}
		}
		x_0 = x_1;
		iter++;
	}
}

int main(void){
	double complex z_1,z_2;
	z_1 = -3.0 - 1.0*I;
	z_2 = 4.0;
	size_t d = 4;
	double complex quotient = newton_iterate(z_1, d);
	//complex res = cdivide( (const) z_1, (const) z_2 );
	newton_res test;
	complex double tr[d];
	find_true_roots(d,tr);
	newton_method(&test, z_1, d);
	printf("Result: root = %f  + i(%f)\n Number of iter: %d \n" ,creal(test.root), cimag(test.root), test.iter_conv );
	
	return 0;
}

