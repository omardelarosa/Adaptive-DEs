// main.c
#include "DE.h"
#include "jDE.h"
#include "CoDE.h"
#include "JADE.h"
#include "SHADE.h"
#include "LSHADE.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// objective function
const double sphere_function(const double * const vector, const int dimension);

int main(void) {
	srand(time(NULL));
	printf("DE: %E\n", run_DE(150000, 100, 0.5, 0.9, sphere_function, 30, -100.0, 100.0));
	printf("jDE: %E\n", run_jDE(150000, 100, sphere_function, 30, -100.0, 100.0));
	printf("CoDE: %E\n", run_CoDE(150000, 100, sphere_function, 30, -100.0, 100.0));
	printf("JADE: %E\n", run_JADE(150000, 100, sphere_function, 30, -100.0, 100.0));
	printf("SHADE: %E\n", run_SHADE(150000, 100, sphere_function, 30, -100.0, 100.0));
	printf("LSHADE: %E\n", run_LSHADE(150000, sphere_function, 30, -100.0, 100.0));
	return 0;
}

const double sphere_function(const double * const vector, const int dimension) {
	double result = 0.0;
	for (int i = 0; i < dimension; ++i)
		result += vector[i] * vector[i];
	return result;
}
