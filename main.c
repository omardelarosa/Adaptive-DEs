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
double *create_initial_population(int population_size, int problem_size);
double *create_initial_fitness_values(int population_size);
void do_something_with_results(const double * population_matrix, const double * fitness_values, const int population_size, const int problem_size);

void run_all(unsigned int seed) {
	srand(seed);
	printf("rand seed: %d\n", seed);
	// Initialization population
	int population_size = 100;
	int problem_size = 30;
	double *population = create_initial_population(population_size, problem_size);
	double *fitness_values = create_initial_fitness_values(population_size);
	printf("DE: %E\n", run_DE(150000, population_size, 0.5, 0.9, sphere_function, problem_size, -100.0, 100.0, NULL, NULL, NULL));
	printf("DE(with provided population): %E\n", run_DE(150000, population_size, 0.5, 0.9, sphere_function, problem_size, -100.0, 100.0, population, fitness_values, do_something_with_results));
	printf("jDE: %E\n", run_jDE(150000, population_size, sphere_function, 30, -100.0, 100.0, NULL, NULL, NULL));
	printf("jDE(with provided population): %E\n", run_DE(150000, population_size, 0.5, 0.9, sphere_function, problem_size, -100.0, 100.0, population, fitness_values, do_something_with_results));
	printf("CoDE: %E\n", run_CoDE(150000, population_size, sphere_function, 30, -100.0, 100.0, NULL, NULL, NULL));
	printf("CoDE(with provided population): %E\n", run_CoDE(150000, population_size, sphere_function, problem_size, -100.0, 100.0, population, fitness_values, do_something_with_results));
	printf("JADE: %E\n", run_JADE(150000, population_size, sphere_function, 30, -100.0, 100.0, NULL, NULL, NULL));
	printf("JADE(with provided population): %E\n", run_JADE(150000, 100, sphere_function, 30, -100.0, 100.0, population, fitness_values, do_something_with_results));
	printf("SHADE: %E\n", run_SHADE(150000, population_size, sphere_function, 30, -100.0, 100.0, NULL, NULL, NULL));
	printf("SHADE(with provided population): %E\n", run_SHADE(150000, 100, sphere_function, 30, -100.0, 100.0, population, fitness_values, do_something_with_results));
	printf("LSHADE: %E\n", run_LSHADE(150000, sphere_function, 30, -100.0, 100.0, NULL, NULL, NULL));
	printf("LSHADE(with provided population): %E\n", run_LSHADE(150000, sphere_function, 30, -100.0, 100.0, population, fitness_values, do_something_with_results));
}

int main(void) {
	run_all(time(NULL));
	return 0;
}

const double sphere_function(const double * const vector, const int dimension) {
	double result = 0.0;
	for (int i = 0; i < dimension; ++i)
		result += vector[i] * vector[i];
	return result;
}

double *create_initial_population(int population_size, int problem_size) {
	int m = population_size;
	int n = problem_size;
	double *arr[m];
	for (int i = 0; i < m; i++)
         arr[i] = (double *)malloc(n * sizeof(double));

	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			arr[i][j] = (double)rand()/RAND_MAX*200.0-100.0;
		}
	}
	return &arr[0][0];
}

double * create_initial_fitness_values(int population_size) {
	int m = population_size;
	double arr[m];
	for (int i = 0; i < population_size; i++) {
		arr[i] = (double)rand()/RAND_MAX*200.0-100.0;
	}
	return (double *)&arr;
}

void do_something_with_results(const double * population_matrix, const double * fitness_values, const int population_size, const int problem_size) {
	printf("ran result callback successfully: population_size: %d, problem_size: %d\n", population_size, problem_size);
}