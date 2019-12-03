// CoDE.c
#include "common.h"
#include "CoDE.h"

static double scaling_factor[3] = { 1.0, 1.0, 0.8 };
static double crossover_rate[3] = { 0.1, 0.9, 0.2 };

static CoDE_individual *get_initialized_population(const int population_size, const int problem_size, const double lower_bound, const double upper_bound);
static CoDE_individual get_initialized_individual(const int problem_size, const double lower_bound, const double upper_bound);
static void terminate_population(CoDE_individual * const population, const int population_size);
static void terminate_individual(const CoDE_individual individual);
static CoDE_individual *get_copied_population(const CoDE_individual * const source_population, const int population_size, const int problem_size);
static CoDE_individual get_copied_individual(const CoDE_individual source_individual, const int problem_size);
static CoDE_individual *get_sorted_population(const CoDE_individual * const population, const int population_size, const int problem_size);
static CoDE_individual *run_recombination(const CoDE_individual * const population, const int population_size,
					  const double(*objective_function)(const double * const, const int),
					  const int problem_size, const double lower_bound, const double upper_bound);
static CoDE_individual *run_selection(const CoDE_individual * const population, const CoDE_individual * const candidates, const int population_size, const int problem_size);
static int get_best_index(const CoDE_individual * const population, const int population_size);

double run_CoDE(const int max_function_evaluations, const int population_size,
		const double(*objective_function)(const double * const, const int), const int problem_size, const double lower_bound, const double upper_bound, double *initial_population, double *fitness_values, void (*results_callback)(const double *population_results, const double *fitness_results, const int population_size, const int problem_size)) {
  // initialization phase
  CoDE_individual *population = get_initialized_population(population_size, problem_size, lower_bound, upper_bound);
  int function_evaluation = 0;
  for (int i = 0; i < population_size; ++i) {
    population[i].fitness = objective_function(population[i].x, problem_size);
    ++function_evaluation;
  }
  // iteration phase
  while (max_function_evaluations > function_evaluation) {
    CoDE_individual * const candidates = run_recombination(population, population_size, objective_function, problem_size, lower_bound, upper_bound);
    function_evaluation += population_size * 3;

    CoDE_individual * const next_population = run_selection(population, candidates, population_size, problem_size);
    terminate_population(population, population_size);
    terminate_population(candidates, population_size);
    population = next_population;
  }
  const double best_fitness = population[get_best_index(population, population_size)].fitness;
  terminate_population(population, population_size);
  return best_fitness;
}

static CoDE_individual *get_initialized_population(const int population_size, const int problem_size, const double lower_bound, const double upper_bound) {
  CoDE_individual * const population = (CoDE_individual *)malloc(sizeof(CoDE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    population[i] = get_initialized_individual(problem_size, lower_bound, upper_bound);
  }
  return population;
}

static CoDE_individual get_initialized_individual(const int problem_size, const double lower_bound, const double upper_bound) {
  CoDE_individual individual;
  individual.x = (double *)malloc(sizeof(double) * problem_size);
  for (int j = 0; j < problem_size; ++j) {
    individual.x[j] = lower_bound + (rand() / (double)RAND_MAX) * (upper_bound - lower_bound);
  }
  individual.fitness = 235711131719.0;
  return individual;
}

static void terminate_population(CoDE_individual * const population, const int population_size) {
  for (int i = 0; i < population_size; ++i) {
    terminate_individual(population[i]);
  }
  free(population);
}

static void terminate_individual(const CoDE_individual individual) {
  free(individual.x);
}

static CoDE_individual *get_copied_population(const CoDE_individual * const source_population, const int population_size, const int problem_size) {
  CoDE_individual * const population = (CoDE_individual *)malloc(sizeof(CoDE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    population[i] = get_copied_individual(source_population[i], problem_size);
  }
  return population;
}

static CoDE_individual get_copied_individual(const CoDE_individual source_individual, const int problem_size) {
  CoDE_individual individual;
  individual.x = (double *)malloc(sizeof(double) * problem_size);
  memcpy(individual.x, source_individual.x, sizeof(double) * problem_size);
  individual.fitness = source_individual.fitness;
  return individual;
}

static CoDE_individual *get_sorted_population(const CoDE_individual * const population, const int population_size, const int problem_size) {
  CoDE_individual * const sorted_population = get_copied_population(population, population_size, problem_size);
  for (int i = 0; i < population_size - 1; ++i) {
    int best_index = get_best_index(sorted_population + i, population_size - i) + i;
    if (best_index != i) {
      CoDE_individual t_individual = sorted_population[i];
      sorted_population[i] = sorted_population[best_index];
      sorted_population[best_index] = t_individual;
    }
  }
  return sorted_population;
}

static CoDE_individual *run_recombination(const CoDE_individual * const population, const int population_size,
					  const double(*objective_function)(const double * const, const int),
					  const int problem_size, const double lower_bound, const double upper_bound) {
  CoDE_individual * const candidates = (CoDE_individual *)malloc(sizeof(CoDE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    CoDE_individual * const t_candidates = get_initialized_population(3, problem_size, lower_bound, upper_bound);
    for (int k = 0; k < 3; ++k) {
      int r1, r2, r3, r4, r5;
      do {
	r1 = rand() % population_size;
      } while (r1 == i);
      do {
	r2 = rand() % population_size;
      } while ((r2 == r1) || (r2 == i));
      do {
	r3 = rand() % population_size;
      } while ((r3 == r2) || (r3 == r1) || (r3 == i));
      do {
	r4 = rand() % population_size;
      } while ((r4 == r3) || (r4 == r2) || (r4 == r1) || (r4 == i));
      do {
	r5 = rand() % population_size;
      } while ((r5 == r4) || (r5 == r3) || (r5 == r2) || (r5 == r1) || (r5 == i));
      const int t = rand() % 3;
      switch (k) {
      case 0:
	t_candidates[k].x = run_rand_1_bin(population[i].x, population[r1].x, population[r2].x, population[r3].x,
					   scaling_factor[t], crossover_rate[t], problem_size, lower_bound, upper_bound);
	t_candidates[k].fitness = objective_function(t_candidates[k].x, problem_size);
	break;
      case 1:
	t_candidates[k].x = run_rand_2_bin(population[i].x, population[r1].x, population[r2].x, population[r3].x, population[r4].x, population[r5].x,
					   scaling_factor[t], crossover_rate[t], problem_size, lower_bound, upper_bound);
	t_candidates[k].fitness = objective_function(t_candidates[k].x, problem_size);
	break;
      case 2:
	t_candidates[k].x = run_current_to_rand_1(population[i].x, population[r1].x, population[r2].x, population[r3].x,
						  scaling_factor[t], problem_size, lower_bound, upper_bound);
	t_candidates[k].fitness = objective_function(t_candidates[k].x, problem_size);
	break;
      }
    }
    candidates[i] = get_copied_individual(t_candidates[get_best_index(t_candidates, 3)], problem_size);
    terminate_population(t_candidates, 3);
  }
  return candidates;
}

static CoDE_individual *run_selection(const CoDE_individual * const population, const CoDE_individual * const candidates, const int population_size, const int problem_size) {
  CoDE_individual * const next_population = (CoDE_individual *)malloc(sizeof(CoDE_individual) * population_size);
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

static int get_best_index(const CoDE_individual * const population, const int population_size) {
  int best_index = 0;
  for (int i = 1; i < population_size; ++i) {
    if (population[i].fitness < population[best_index].fitness) {
      best_index = i;
    }
  }
  return best_index;
}
