// DE.c
#include "common.h"
#include <stdio.h>
#include "DE.h"

static DE_individual *get_initialized_population(const int population_size, const int problem_size, const double lower_bound, const double upper_bound);
static DE_individual *get_initialized_population_from_array(const int population_size, const int problem_size, double *initialized_population, double*fitness_values);
static DE_individual get_initialized_individual(const int problem_size, const double lower_bound, const double upper_bound);
static void terminate_population(DE_individual * const population, const int population_size);
static void terminate_individual(const DE_individual individual);
static DE_individual *get_copied_population(const DE_individual * const source_population, const int population_size, const int problem_size);
static DE_individual get_copied_individual(const DE_individual source_individual, const int problem_size);
static DE_individual *get_sorted_population(const DE_individual * const population, const int population_size, const int problem_size);
static DE_individual *run_recombination(const DE_individual * const population, const int population_size, const double scaling_factor, const double crossover_rate,
					const int problem_size, const double lower_bound, const double upper_bound);
static DE_individual *run_selection(const DE_individual * const population, const DE_individual * const candidates, const int population_size, const int problem_size);
static int get_best_index(const DE_individual * const population, const int population_size);

double run_DE(const int max_function_evaluations, const int population_size, const double scaling_factor, const double crossover_rate,
	      const double(*objective_function)(const double * const, const int), const int problem_size, const double lower_bound, const double upper_bound) {
  // initialization phase
  DE_individual *population = get_initialized_population(population_size, problem_size, lower_bound, upper_bound);
  int function_evaluation = 0;
  for (int i = 0; i < population_size; ++i) {
    population[i].fitness = objective_function(population[i].x, problem_size);
    ++function_evaluation;
  }
  // iteration phase
  while (max_function_evaluations > function_evaluation) {
    DE_individual * const candidates = run_recombination(population, population_size, scaling_factor, crossover_rate, problem_size, lower_bound, upper_bound);
    for (int i = 0; i < population_size; ++i) {
      candidates[i].fitness = objective_function(candidates[i].x, problem_size);
      ++function_evaluation;
    }

    DE_individual * const next_population = run_selection(population, candidates, population_size, problem_size);
    terminate_population(population, population_size);
    terminate_population(candidates, population_size);
    population = next_population;
  }
  const double best_fitness = population[get_best_index(population, population_size)].fitness;
  terminate_population(population, population_size);
  return best_fitness;
}

double run_DE_with_population_provided(
    const int max_function_evaluations, const int population_size,
    const double scaling_factor, const double crossover_rate,
    const double (*objective_function)(const double *const, const int),
    const int problem_size, const double lower_bound, const double upper_bound,
    double *initial_population, double *fitness_values, double *population_results, double *fitness_results) {
  // initialization phase
  DE_individual *population = get_initialized_population_from_array(population_size, problem_size, initial_population, fitness_values);
  // return 1.0;
  int function_evaluation = 0;
  for (int i = 0; i < population_size; ++i) {
    population[i].fitness = objective_function(population[i].x, problem_size);
    ++function_evaluation;
  }
  // iteration phase
  while (max_function_evaluations > function_evaluation) {
    DE_individual * const candidates = run_recombination(population, population_size, scaling_factor, crossover_rate, problem_size, lower_bound, upper_bound);
    for (int i = 0; i < population_size; ++i) {
      candidates[i].fitness = objective_function(candidates[i].x, problem_size);
      ++function_evaluation;
    }

    DE_individual * const next_population = run_selection(population, candidates, population_size, problem_size);
    terminate_population(population, population_size);
    terminate_population(candidates, population_size);
    population = next_population;
  }

  const double best_fitness = population[get_best_index(population, population_size)].fitness;

  // Extract fitness values and final population
  if (population_results != NULL && fitness_results != NULL) {
    double *population_matrix_results[population_size];
    double fitness_values_results[population_size];
    for (int i = 0; i < population_size; i++)
          population_matrix_results[i] = (double *)malloc(problem_size * sizeof(double));

    for (int i = 0; i < population_size; i++) {
      for (int j = 0; j < problem_size; j++) {
        population_matrix_results[i][j] = population[i].x[j];
      }
      fitness_values_results[i] = population[i].fitness;
    }

    population_results = &population_matrix_results[0][0];
    fitness_results = (double *)&fitness_values_results;
  }


  terminate_population(population, population_size);
  return best_fitness;
}

static DE_individual *get_initialized_population(const int population_size, const int problem_size, const double lower_bound, const double upper_bound) {
  DE_individual * const population = (DE_individual *)malloc(sizeof(DE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    population[i] = get_initialized_individual(problem_size, lower_bound, upper_bound);
  }
  return population;
}

static DE_individual *get_initialized_population_from_array(const int population_size, const int problem_size, double *initial_population, double *fitness_values) {
  // Creating population array pointer
  DE_individual * const population = (DE_individual *)malloc(sizeof(DE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    static DE_individual individual;
    individual.x = (double *)malloc(problem_size * sizeof(double));
    // Transfer values from 2D array to population array
    for (int j = 0; j < problem_size; j++) {
      individual.x[j] = *(initial_population + i * problem_size + j);
    }
    individual.fitness = (double)fitness_values[i];
    population[i] = individual;
  }
  return population;
}


static DE_individual get_initialized_individual(const int problem_size, const double lower_bound, const double upper_bound) {
  DE_individual individual;
  individual.x = (double *)malloc(sizeof(double) * problem_size);
  for (int j = 0; j < problem_size; ++j) {
    individual.x[j] = lower_bound + (rand() / (double)RAND_MAX) * (upper_bound - lower_bound);
  }
  individual.fitness = 235711131719.0;
  return individual;
}

static void terminate_population(DE_individual * const population, const int population_size) {
  for (int i = 0; i < population_size; ++i) {
    terminate_individual(population[i]);
  }
  free(population);
}

static void terminate_individual(const DE_individual individual) {
  free(individual.x);
}

static DE_individual *get_copied_population(const DE_individual * const source_population, const int population_size, const int problem_size) {
  DE_individual * const population = (DE_individual *)malloc(sizeof(DE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    population[i] = get_copied_individual(source_population[i], problem_size);
  }
  return population;
}

static DE_individual get_copied_individual(const DE_individual source_individual, const int problem_size) {
  DE_individual individual;
  individual.x = (double *)malloc(sizeof(double) * problem_size);
  memcpy(individual.x, source_individual.x, sizeof(double) * problem_size);
  individual.fitness = source_individual.fitness;
  return individual;
}

static DE_individual *get_sorted_population(const DE_individual * const population, const int population_size, const int problem_size) {
  DE_individual * const sorted_population = get_copied_population(population, population_size, problem_size);
  for (int i = 0; i < population_size - 1; ++i) {
    int best_index = get_best_index(sorted_population + i, population_size - i) + i;
    if (best_index != i) {
      DE_individual t_individual = sorted_population[i];
      sorted_population[i] = sorted_population[best_index];
      sorted_population[best_index] = t_individual;
    }
  }
  return sorted_population;
}

static DE_individual *run_recombination(const DE_individual * const population, const int population_size, const double scaling_factor, const double crossover_rate,
					const int problem_size, const double lower_bound, const double upper_bound) {
  DE_individual * const candidates = (DE_individual *)malloc(sizeof(DE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    int r1, r2, r3;
    do {
      r1 = rand() % population_size;
    } while (r1 == i);
    do {
      r2 = rand() % population_size;
    } while ((r2 == r1) || (r2 == i));
    do {
      r3 = rand() % population_size;
    } while ((r3 == r2) || (r3 == r1) || (r3 == i));
    candidates[i].x = run_rand_1_bin(population[i].x, population[r1].x, population[r2].x, population[r3].x, scaling_factor, crossover_rate, problem_size, lower_bound, upper_bound);
  }
  return candidates;
}

static DE_individual *run_selection(const DE_individual * const population, const DE_individual * const candidates, const int population_size, const int problem_size) {
  DE_individual * const next_population = (DE_individual *)malloc(sizeof(DE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    if (candidates[i].fitness <= population[i].fitness) {
      next_population[i] = get_copied_individual(candidates[i], problem_size);
    }
    else {
      next_population[i] = get_copied_individual(population[i], problem_size);
    }
  }
  return next_population;
}

static int get_best_index(const DE_individual * const population, const int population_size) {
  int best_index = 0;
  for (int i = 1; i < population_size; ++i) {
    if (population[i].fitness < population[best_index].fitness) {
      best_index = i;
    }
  }
  return best_index;
}
