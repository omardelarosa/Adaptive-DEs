// JADE.c
#include "common.h"
#include "JADE.h"

static const double scaling_factor = 0.5;
static const double crossover_rate = 0.5;
static const double p_best_rate = 0.05;
static const double archive_rate = 1.0;

static JADE_individual *get_initialized_population(const int population_size, const int problem_size, const double lower_bound, const double upper_bound);
static JADE_individual *get_initialized_population_from_array(const int population_size, const int problem_size, double *initialized_population, double*fitness_values);
static JADE_individual *get_initialized_population_from_array(const int population_size, const int problem_size, double *initialized_population, double*fitness_values);
static JADE_individual get_initialized_individual(const int problem_size, const double lower_bound, const double upper_bound);
static void terminate_population(JADE_individual * const population, const int population_size);
static void terminate_individual(const JADE_individual individual);
static JADE_individual *get_copied_population(const JADE_individual * const source_population, const int population_size, const int problem_size);
static JADE_individual get_copied_individual(const JADE_individual source_individual, const int problem_size);
static JADE_individual *get_sorted_population(const JADE_individual * const population, const int population_size, const int problem_size);
static JADE_individual *run_recombination(const JADE_individual * const population, const int population_size,
					  const int problem_size, const double lower_bound, const double upper_bound,
					  const JADE_individual * const archive, const int archive_size, const int archive_cnt,
					  const double mu_scaling_factor, const double mu_crossover_rate);
static JADE_parameter_control_results parameter_control(const double scaling_factor, const double crossover_rate, const double mu_scaling_factor, const double mu_crossover_rate);
static double get_updated_mu_scaling_factor(const double mu_scaling_factor, const double c, const double * const success_scaling_factors, const int success_cnt);
static double get_updated_mu_crossover_rate(const double mu_crossover_rate, const double c, const double * const success_crossover_rates, const int success_cnt);
static JADE_selection_results run_selection(const JADE_individual * const population, const JADE_individual * const candidates, const int population_size, const int problem_size,
					    const JADE_individual * const archive, const int archive_size, const int archive_cnt,
					    const double mu_scaling_factor, const double mu_crossover_rate);
static int get_best_index(const JADE_individual * const population, const int population_size);

double run_JADE(const int max_function_evaluations, const int population_size,
		const double(*objective_function)(const double * const, const int), const int problem_size, const double lower_bound, const double upper_bound, double *initial_population, double *fitness_values, void (*results_callback)(const double *population_results, const double *fitness_results, const int population_size, const int problem_size)) {
  // initialization phase
  JADE_individual *population;

  // initialization phase
  if (initial_population != NULL && fitness_values != NULL) {
    population = get_initialized_population_from_array(population_size, problem_size, initial_population, fitness_values);
  } else {
    population = get_initialized_population(population_size, problem_size, lower_bound, upper_bound);
  }

  int function_evaluation = 0;

  const int archive_size = (int)round(population_size * archive_rate);
  JADE_individual *archive = get_initialized_population(archive_size, problem_size, lower_bound, upper_bound);
  double mu_scaling_factor = scaling_factor;
  double mu_crossover_rate = crossover_rate;
  int archive_cnt = 0;

  JADE_individual * const sorted_population = get_sorted_population(population, population_size, problem_size);
  terminate_population(population, population_size);
  population = sorted_population;
  // iteration phase
  while (max_function_evaluations > function_evaluation) {
    JADE_individual * const candidates = run_recombination(population, population_size, problem_size, lower_bound, upper_bound, archive, archive_size, archive_cnt,
							   mu_scaling_factor, mu_crossover_rate);
    for (int i = 0; i < population_size; ++i) {
      candidates[i].fitness = objective_function(candidates[i].x, problem_size);
      ++function_evaluation;
    }

    const JADE_selection_results results = run_selection(population, candidates, population_size, problem_size, archive, archive_size, archive_cnt,
							 mu_scaling_factor, mu_crossover_rate);
    terminate_population(population, population_size);
    terminate_population(candidates, population_size);
    terminate_population(archive, archive_cnt);
    population = results.population;
    archive = results.archive;
    mu_scaling_factor = results.mu_scaling_factor;
    mu_crossover_rate = results.mu_crossover_rate;
    archive_cnt = results.archive_cnt;
  }
  const double best_fitness = population[get_best_index(population, population_size)].fitness;

  // Extract fitness values and final population
  if (results_callback != NULL) {
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
    results_callback(&population_matrix_results[0][0], (double*)&fitness_values_results, population_size, problem_size);
  }

  terminate_population(population, population_size);
  terminate_population(archive, archive_cnt);
  return best_fitness;
}

static JADE_individual *get_initialized_population(const int population_size, const int problem_size, const double lower_bound, const double upper_bound) {
  JADE_individual * const population = (JADE_individual *)malloc(sizeof(JADE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    population[i] = get_initialized_individual(problem_size, lower_bound, upper_bound);
  }
  return population;
}

static JADE_individual *get_initialized_population_from_array(const int population_size, const int problem_size, double *initial_population, double *fitness_values) {
  // Creating population array pointer
  JADE_individual * const population = (JADE_individual *)malloc(sizeof(JADE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    static JADE_individual individual;
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


static JADE_individual get_initialized_individual(const int problem_size, const double lower_bound, const double upper_bound) {
  JADE_individual individual;
  individual.x = (double *)malloc(sizeof(double) * problem_size);
  for (int j = 0; j < problem_size; ++j) {
    individual.x[j] = lower_bound + (rand() / (double)RAND_MAX) * (upper_bound - lower_bound);
  }
  individual.fitness = 235711131719.0;
  individual.scaling_factor = scaling_factor;
  individual.crossover_rate = crossover_rate;
  return individual;
}

static void terminate_population(JADE_individual * const population, const int population_size) {
  for (int i = 0; i < population_size; ++i) {
    terminate_individual(population[i]);
  }
  free(population);
}

static void terminate_individual(const JADE_individual individual) {
  free(individual.x);
}

static JADE_individual *get_copied_population(const JADE_individual * const source_population, const int population_size, const int problem_size) {
  JADE_individual * const population = (JADE_individual *)malloc(sizeof(JADE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    population[i] = get_copied_individual(source_population[i], problem_size);
  }
  return population;
}

static JADE_individual get_copied_individual(const JADE_individual source_individual, const int problem_size) {
  JADE_individual individual;
  individual.x = (double *)malloc(sizeof(double) * problem_size);
  memcpy(individual.x, source_individual.x, sizeof(double) * problem_size);
  individual.fitness = source_individual.fitness;
  individual.scaling_factor = source_individual.scaling_factor;
  individual.crossover_rate = source_individual.crossover_rate;
  return individual;
}

static JADE_individual *get_sorted_population(const JADE_individual * const population, const int population_size, const int problem_size) {
  JADE_individual * const sorted_population = get_copied_population(population, population_size, problem_size);
  for (int i = 0; i < population_size - 1; ++i) {
    int best_index = get_best_index(sorted_population + i, population_size - i) + i;
    if (best_index != i) {
      JADE_individual t_individual = sorted_population[i];
      sorted_population[i] = sorted_population[best_index];
      sorted_population[best_index] = t_individual;
    }
  }
  return sorted_population;
}

static JADE_individual *run_recombination(const JADE_individual * const population, const int population_size,
					  const int problem_size, const double lower_bound, const double upper_bound,
					  const JADE_individual * const archive, const int archive_size, const int archive_cnt,
					  const double mu_scaling_factor, const double mu_crossover_rate) {
  JADE_individual * const candidates = (JADE_individual *)malloc(sizeof(JADE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    const int p_best = (int)round(population_size * p_best_rate);
    const int p_best_index = rand() % p_best;
    int r1, r2;
    do {
      r1 = rand() % population_size;
    } while ((r1 == p_best_index) || (r1 == i));
    do {
      r2 = rand() % (population_size + archive_cnt);
    } while ((r2 == p_best_index) || (r2 == r1) || (r2 == i));
    const JADE_parameter_control_results results = parameter_control(population[i].scaling_factor, population[i].crossover_rate, mu_scaling_factor, mu_crossover_rate);
    candidates[i].scaling_factor = results.scaling_factor;
    candidates[i].crossover_rate = results.crossover_rate;
    if (population_size > r2) {
      candidates[i].x = run_target_to_best_1_bin(population[i].x, population[p_best_index].x, population[r1].x, population[r2].x,
						 candidates[i].scaling_factor, candidates[i].crossover_rate, problem_size, lower_bound, upper_bound);
    }
    else {
      candidates[i].x = run_target_to_best_1_bin(population[i].x, population[p_best_index].x, population[r1].x, archive[r2 - population_size].x,
						 candidates[i].scaling_factor, candidates[i].crossover_rate, problem_size, lower_bound, upper_bound);
    }
  }
  return candidates;
}

static JADE_parameter_control_results parameter_control(const double scaling_factor, const double crossover_rate, const double mu_scaling_factor, const double mu_crossover_rate) {
  JADE_parameter_control_results results;
  do {
    results.scaling_factor = generate_Cauchy(mu_scaling_factor, 0.1);
    results.scaling_factor = fmin(1.0, results.scaling_factor);
  } while (0.0 >= results.scaling_factor);
  results.crossover_rate = generate_Gaussian(mu_crossover_rate, 0.1);
  results.crossover_rate = fmin(1.0, fmax(0.0, results.crossover_rate));
  return results;
}

static double get_updated_mu_scaling_factor(const double mu_scaling_factor, const double c, const double * const success_scaling_factors, const int success_cnt) {
  return (1.0 - c) * mu_scaling_factor + c * Lehmer_mean(success_scaling_factors, success_cnt);
}

static double get_updated_mu_crossover_rate(const double mu_crossover_rate, const double c, const double * const success_crossover_rates, const int success_cnt) {
  return (1.0 - c) * mu_crossover_rate + c * arithmetic_mean(success_crossover_rates, success_cnt);
}

static JADE_selection_results run_selection(const JADE_individual * const population, const JADE_individual * const candidates, const int population_size, const int problem_size,
					    const JADE_individual * const archive, const int archive_size, const int archive_cnt,
					    const double mu_scaling_factor, const double mu_crossover_rate) {
  JADE_individual * const next_population = (JADE_individual *)malloc(sizeof(JADE_individual) * population_size);
  JADE_individual * const next_archive = (JADE_individual *)malloc(sizeof(JADE_individual) * archive_size);
  double next_mu_scaling_factor = mu_scaling_factor;
  double next_mu_crossover_rate = mu_crossover_rate;
  int next_archive_cnt = archive_cnt;
  double * const success_scaling_factors = (double *)malloc(sizeof(double) * population_size);
  double * const success_crossover_rates = (double *)malloc(sizeof(double) * population_size);
  int success_cnt = 0;
  for (int i = 0; i < archive_size; ++i) {
    next_archive[i] = get_copied_individual(archive[i], problem_size);
  }
  memset(success_scaling_factors, -1.0, sizeof(double) * population_size);
  memset(success_crossover_rates, -1.0, sizeof(double) * population_size);
  for (int i = 0; i < population_size; ++i) {
    if (candidates[i].fitness == population[i].fitness) {
      next_population[i] = get_copied_individual(candidates[i], problem_size);
    }
    else if (candidates[i].fitness < population[i].fitness) {
      next_population[i] = get_copied_individual(candidates[i], problem_size);

      if (next_archive_cnt < archive_size) {
	next_archive[next_archive_cnt] = get_copied_individual(population[i], problem_size);
	++next_archive_cnt;
      }
      else {
	const int t = rand() % archive_size;
	terminate_individual(next_archive[t]);
	next_archive[t] = get_copied_individual(population[i], problem_size);
      }
      success_scaling_factors[success_cnt] = candidates[i].scaling_factor;
      success_crossover_rates[success_cnt] = candidates[i].crossover_rate;
      ++success_cnt;
    }
    else {
      next_population[i] = get_copied_individual(population[i], problem_size);
    }
  }
  if (0 != success_cnt) {
    next_mu_scaling_factor = get_updated_mu_scaling_factor(mu_scaling_factor, 0.1, success_scaling_factors, success_cnt);
    next_mu_crossover_rate = get_updated_mu_crossover_rate(mu_crossover_rate, 0.1, success_crossover_rates, success_cnt);
  }
  free(success_scaling_factors);
  free(success_crossover_rates);
  JADE_individual * const sorted_next_population = get_sorted_population(next_population, population_size, problem_size);
  terminate_population(next_population, population_size);
  return (JADE_selection_results) { sorted_next_population, next_archive, next_mu_scaling_factor, next_mu_crossover_rate, next_archive_cnt };
}

static int get_best_index(const JADE_individual * const population, const int population_size) {
  int best_index = 0;
  for (int i = 1; i < population_size; ++i) {
    if (population[i].fitness < population[best_index].fitness) {
      best_index = i;
    }
  }
  return best_index;
}
