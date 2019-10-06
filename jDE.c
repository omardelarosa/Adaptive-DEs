// jDE.c
#include "common.h"
#include "jDE.h"

static const double scaling_factor = 0.5;
static const double crossover_rate = 0.9;

static jDE_individual *get_initialized_population(const int population_size, const int problem_size, const double lower_bound, const double upper_bound);
static jDE_individual get_initialized_individual(const int problem_size, const double lower_bound, const double upper_bound);
static void terminate_population(jDE_individual * const population, const int population_size);
static void terminate_individual(const jDE_individual individual);
static jDE_individual *get_copied_population(const jDE_individual * const source_population, const int population_size, const int problem_size);
static jDE_individual get_copied_individual(const jDE_individual source_individual, const int problem_size);
static jDE_individual *get_sorted_population(const jDE_individual * const population, const int population_size, const int problem_size);
static jDE_individual *run_recombination(const jDE_individual * const population, const int population_size,
					 const int problem_size, const double lower_bound, const double upper_bound);
static jDE_parameter_control_results parameter_control(const double scaling_factor, const double crossover_rate);
static jDE_individual *run_selection(const jDE_individual * const population, const jDE_individual * const candidates, const int population_size, const int problem_size);
static int get_best_index(const jDE_individual * const population, const int population_size);

double run_jDE(const int max_function_evaluations, const int population_size,
	       const double(*objective_function)(const double * const, const int), const int problem_size, const double lower_bound, const double upper_bound) {
  // initialization phase
  jDE_individual *population = get_initialized_population(population_size, problem_size, lower_bound, upper_bound);
  int function_evaluation = 0;
  for (int i = 0; i < population_size; ++i) {
    population[i].fitness = objective_function(population[i].x, problem_size);
    ++function_evaluation;
  }
  // iteration phase
  while (max_function_evaluations > function_evaluation) {
    jDE_individual * const candidates = run_recombination(population, population_size, problem_size, lower_bound, upper_bound);
    for (int i = 0; i < population_size; ++i) {
      candidates[i].fitness = objective_function(candidates[i].x, problem_size);
      ++function_evaluation;
    }

    jDE_individual * const next_population = run_selection(population, candidates, population_size, problem_size);
    terminate_population(population, population_size);
    terminate_population(candidates, population_size);
    population = next_population;
  }
  const double best_fitness = population[get_best_index(population, population_size)].fitness;
  terminate_population(population, population_size);
  return best_fitness;
}

static jDE_individual *get_initialized_population(const int population_size, const int problem_size, const double lower_bound, const double upper_bound) {
  jDE_individual * const population = (jDE_individual *)malloc(sizeof(jDE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    population[i] = get_initialized_individual(problem_size, lower_bound, upper_bound);
  }
  return population;
}

static jDE_individual get_initialized_individual(const int problem_size, const double lower_bound, const double upper_bound) {
  jDE_individual individual;
  individual.x = (double *)malloc(sizeof(double) * problem_size);
  for (int j = 0; j < problem_size; ++j) {
    individual.x[j] = lower_bound + (rand() / (double)RAND_MAX) * (upper_bound - lower_bound);
  }
  individual.fitness = 235711131719.0;
  individual.scaling_factor = scaling_factor;
  individual.crossover_rate = crossover_rate;
  return individual;
}

static void terminate_population(jDE_individual * const population, const int population_size) {
  for (int i = 0; i < population_size; ++i) {
    terminate_individual(population[i]);
  }
  free(population);
}

static void terminate_individual(const jDE_individual individual) {
  free(individual.x);
}

static jDE_individual *get_copied_population(const jDE_individual * const source_population, const int population_size, const int problem_size) {
  jDE_individual * const population = (jDE_individual *)malloc(sizeof(jDE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    population[i] = get_copied_individual(source_population[i], problem_size);
  }
  return population;
}

static jDE_individual get_copied_individual(const jDE_individual source_individual, const int problem_size) {
  jDE_individual individual;
  individual.x = (double *)malloc(sizeof(double) * problem_size);
  memcpy(individual.x, source_individual.x, sizeof(double) * problem_size);
  individual.fitness = source_individual.fitness;
  individual.scaling_factor = source_individual.scaling_factor;
  individual.crossover_rate = source_individual.crossover_rate;
  return individual;
}

static jDE_individual *get_sorted_population(const jDE_individual * const population, const int population_size, const int problem_size) {
  jDE_individual * const sorted_population = get_copied_population(population, population_size, problem_size);
  for (int i = 0; i < population_size - 1; ++i) {
    int best_index = get_best_index(sorted_population + i, population_size - i) + i;
    if (best_index != i) {
      jDE_individual t_individual = sorted_population[i];
      sorted_population[i] = sorted_population[best_index];
      sorted_population[best_index] = t_individual;
    }
  }
  return sorted_population;
}

static jDE_individual *run_recombination(const jDE_individual * const population, const int population_size,
					 const int problem_size, const double lower_bound, const double upper_bound) {
  jDE_individual * const candidates = (jDE_individual *)malloc(sizeof(jDE_individual) * population_size);
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
    const jDE_parameter_control_results results = parameter_control(population[i].scaling_factor, population[i].crossover_rate);
    candidates[i].scaling_factor = results.scaling_factor;
    candidates[i].crossover_rate = results.crossover_rate;
    candidates[i].x = run_rand_1_bin(population[i].x, population[r1].x, population[r2].x, population[r3].x,
				     candidates[i].scaling_factor, candidates[i].crossover_rate, problem_size, lower_bound, upper_bound);
  }
  return candidates;
}

static jDE_parameter_control_results parameter_control(const double scaling_factor, const double crossover_rate) {
  jDE_parameter_control_results results;
  if ((rand() / (double)RAND_MAX) <= 0.1) {
    results.scaling_factor = 0.1 + (rand() / (double)RAND_MAX) * (0.9 - 0.1);
  }
  else {
    results.scaling_factor = scaling_factor;
  }
  if ((rand() / (double)RAND_MAX) <= 0.1) {
    results.crossover_rate = rand() / (double)RAND_MAX;
  }
  else {
    results.crossover_rate = crossover_rate;
  }
  return results;
}

static jDE_individual *run_selection(const jDE_individual * const population, const jDE_individual * const candidates, const int population_size, const int problem_size) {
  jDE_individual * const next_population = (jDE_individual *)malloc(sizeof(jDE_individual) * population_size);
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

static int get_best_index(const jDE_individual * const population, const int population_size) {
  int best_index = 0;
  for (int i = 1; i < population_size; ++i) {
    if (population[i].fitness < population[best_index].fitness) {
      best_index = i;
    }
  }
  return best_index;
}
