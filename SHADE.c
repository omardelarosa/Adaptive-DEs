// SHADE.c
#include "common.h"
#include "SHADE.h"

static const double scaling_factor = 0.5;
static const double crossover_rate = 0.5;
static const double p_best_rate = 0.1;
static const double archive_rate = 2.0;

static SHADE_individual *get_initialized_population(const int population_size, const int problem_size, const double lower_bound, const double upper_bound);
static SHADE_individual get_initialized_individual(const int problem_size, const double lower_bound, const double upper_bound);
static void terminate_population(SHADE_individual * const population, const int population_size);
static void terminate_individual(const SHADE_individual individual);
static SHADE_individual *get_copied_population(const SHADE_individual * const source_population, const int population_size, const int problem_size);
static SHADE_individual get_copied_individual(const SHADE_individual source_individual, const int problem_size);
static SHADE_individual *get_sorted_population(const SHADE_individual * const population, const int population_size, const int problem_size);
static SHADE_individual *run_recombination(const SHADE_individual * const population, const int population_size,
					   const int problem_size, const double lower_bound, const double upper_bound,
					   const SHADE_individual * const archive, const int archive_size, const int archive_cnt,
					   const double * const mu_scaling_factor, const double * const mu_crossover_rate, const int memory_size);
static SHADE_parameter_control_results parameter_control(const double scaling_factor, const double crossover_rate, const double mu_scaling_factor, const double mu_crossover_rate);
static SHADE_selection_results run_selection(const SHADE_individual * const population, const SHADE_individual * const candidates, const int population_size, const int problem_size,
					     const SHADE_individual * const archive, const int archive_size, const int archive_cnt,
					     const double mu_scaling_factor, const double mu_crossover_rate);
static int get_best_index(const SHADE_individual * const population, const int population_size);

double run_SHADE(const int max_function_evaluations, const int population_size,
		 const double(*objective_function)(const double * const, const int), const int problem_size, const double lower_bound, const double upper_bound, double *initial_population, double *fitness_values, void (*results_callback)(const double *population_results, const double *fitness_results, const int population_size, const int problem_size)) {
  // initialization phase
  SHADE_individual *population = get_initialized_population(population_size, problem_size, lower_bound, upper_bound);
  int function_evaluation = 0;
  for (int i = 0; i < population_size; ++i) {
    population[i].fitness = objective_function(population[i].x, problem_size);
    ++function_evaluation;
  }
  const int archive_size = (int)round(population_size * archive_rate);
  const int memory_size = problem_size;
  SHADE_individual *archive = get_initialized_population(archive_size, problem_size, lower_bound, upper_bound);
  double *mu_scaling_factor = (double *)malloc(sizeof(double) * memory_size);
  double *mu_crossover_rate = (double *)malloc(sizeof(double) * memory_size);
  memset(mu_scaling_factor, scaling_factor, sizeof(double) * memory_size);
  memset(mu_crossover_rate, crossover_rate, sizeof(double) * memory_size);
  int archive_cnt = 0;
  int memory_pos = 0;

  SHADE_individual * const sorted_population = get_sorted_population(population, population_size, problem_size);
  terminate_population(population, population_size);
  population = sorted_population;
  // iteration phase
  while (max_function_evaluations > function_evaluation) {
    SHADE_individual * const candidates = run_recombination(population, population_size, problem_size, lower_bound, upper_bound, archive, archive_size, archive_cnt,
							    mu_scaling_factor, mu_crossover_rate, memory_size);
    for (int i = 0; i < population_size; ++i) {
      candidates[i].fitness = objective_function(candidates[i].x, problem_size);
      ++function_evaluation;
    }

    const SHADE_selection_results results = run_selection(population, candidates, population_size, problem_size, archive, archive_size, archive_cnt,
							  mu_scaling_factor[memory_pos % memory_size], mu_crossover_rate[memory_pos % memory_size]);
    terminate_population(population, population_size);
    terminate_population(candidates, population_size);
    terminate_population(archive, archive_cnt);
    population = results.population;
    archive = results.archive;
    mu_scaling_factor[memory_pos % memory_size] = results.mu_scaling_factor;
    mu_crossover_rate[memory_pos % memory_size] = results.mu_crossover_rate;
    archive_cnt = results.archive_cnt;
    ++memory_pos;
  }
  const double best_fitness = population[get_best_index(population, population_size)].fitness;
  terminate_population(population, population_size);
  terminate_population(archive, archive_cnt);
  free(mu_scaling_factor);
  free(mu_crossover_rate);
  return best_fitness;
}

static SHADE_individual *get_initialized_population(const int population_size, const int problem_size, const double lower_bound, const double upper_bound) {
  SHADE_individual * const population = (SHADE_individual *)malloc(sizeof(SHADE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    population[i] = get_initialized_individual(problem_size, lower_bound, upper_bound);
  }
  return population;
}

static SHADE_individual get_initialized_individual(const int problem_size, const double lower_bound, const double upper_bound) {
  SHADE_individual individual;
  individual.x = (double *)malloc(sizeof(double) * problem_size);
  for (int j = 0; j < problem_size; ++j) {
    individual.x[j] = lower_bound + (rand() / (double)RAND_MAX) * (upper_bound - lower_bound);
  }
  individual.fitness = 235711131719.0;
  individual.scaling_factor = scaling_factor;
  individual.crossover_rate = crossover_rate;
  return individual;
}

static void terminate_population(SHADE_individual * const population, const int population_size) {
  for (int i = 0; i < population_size; ++i) {
    terminate_individual(population[i]);
  }
  free(population);
}

static void terminate_individual(const SHADE_individual individual) {
  free(individual.x);
}

static SHADE_individual *get_copied_population(const SHADE_individual * const source_population, const int population_size, const int problem_size) {
  SHADE_individual * const population = (SHADE_individual *)malloc(sizeof(SHADE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    population[i] = get_copied_individual(source_population[i], problem_size);
  }
  return population;
}

static SHADE_individual get_copied_individual(const SHADE_individual source_individual, const int problem_size) {
  SHADE_individual individual;
  individual.x = (double *)malloc(sizeof(double) * problem_size);
  memcpy(individual.x, source_individual.x, sizeof(double) * problem_size);
  individual.fitness = source_individual.fitness;
  individual.scaling_factor = source_individual.scaling_factor;
  individual.crossover_rate = source_individual.crossover_rate;
  return individual;
}

static SHADE_individual *get_sorted_population(const SHADE_individual * const population, const int population_size, const int problem_size) {
  SHADE_individual * const sorted_population = get_copied_population(population, population_size, problem_size);
  for (int i = 0; i < population_size - 1; ++i) {
    int best_index = get_best_index(sorted_population + i, population_size - i) + i;
    if (best_index != i) {
      SHADE_individual t_individual = sorted_population[i];
      sorted_population[i] = sorted_population[best_index];
      sorted_population[best_index] = t_individual;
    }
  }
  return sorted_population;
}

static SHADE_individual *run_recombination(const SHADE_individual * const population, const int population_size,
					   const int problem_size, const double lower_bound, const double upper_bound,
					   const SHADE_individual * const archive, const int archive_size, const int archive_cnt,
					   const double * const mu_scaling_factor, const double * const mu_crossover_rate, const int memory_size) {
  SHADE_individual * const candidates = (SHADE_individual *)malloc(sizeof(SHADE_individual) * population_size);
  for (int i = 0; i < population_size; ++i) {
    const int p_best = (int)round(population_size * p_best_rate);
    const int p_best_index = rand() % p_best;
    int r1, r2;
    do {
      r1 = rand() % population_size;
    } while (r1 == i);
    do {
      r2 = rand() % (population_size + archive_cnt);
    } while ((r2 == r1) || (r2 == i));
    const int t = rand() % memory_size;
    const SHADE_parameter_control_results results = parameter_control(population[i].scaling_factor, population[i].crossover_rate, mu_scaling_factor[t], mu_crossover_rate[t]);
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

static SHADE_parameter_control_results parameter_control(const double scaling_factor, const double crossover_rate, const double mu_scaling_factor, const double mu_crossover_rate) {
  SHADE_parameter_control_results results;
  do {
    results.scaling_factor = generate_Cauchy(mu_scaling_factor, 0.1);
    results.scaling_factor = fmin(1.0, results.scaling_factor);
  } while (0.0 >= results.scaling_factor);
  if (-1.0 == mu_crossover_rate) {
    results.crossover_rate = 0.0;
  }
  else {
    results.crossover_rate = generate_Gaussian(mu_crossover_rate, 0.1);
    results.crossover_rate = fmin(1.0, fmax(0.0, results.crossover_rate));
  }
  return results;
}

static SHADE_selection_results run_selection(const SHADE_individual * const population, const SHADE_individual * const candidates, const int population_size, const int problem_size,
					     const SHADE_individual * const archive, const int archive_size, const int archive_cnt,
					     const double mu_scaling_factor, const double mu_crossover_rate) {
  SHADE_individual * const next_population = (SHADE_individual *)malloc(sizeof(SHADE_individual) * population_size);
  SHADE_individual * const next_archive = (SHADE_individual *)malloc(sizeof(SHADE_individual) * archive_size);
  double next_mu_scaling_factor = mu_scaling_factor;
  double next_mu_crossover_rate = mu_crossover_rate;
  int next_archive_cnt = archive_cnt;
  double * const success_scaling_factors = (double *)malloc(sizeof(double) * population_size);
  double * const success_crossover_rates = (double *)malloc(sizeof(double) * population_size);
  double * const fitness_differences = (double *)malloc(sizeof(double) * population_size);
  int success_cnt = 0;
  for (int i = 0; i < archive_size; ++i) {
    next_archive[i] = get_copied_individual(archive[i], problem_size);
  }
  memset(success_scaling_factors, -1.0, sizeof(double) * population_size);
  memset(success_crossover_rates, -1.0, sizeof(double) * population_size);
  memset(fitness_differences, 0.0, sizeof(double) * population_size);
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
      fitness_differences[success_cnt] = fabs(candidates[i].fitness - population[i].fitness);
      ++success_cnt;
    }
    else {
      next_population[i] = get_copied_individual(population[i], problem_size);
    }
  }
  if (0 != success_cnt) {
    double sum = 0.0;
    double sum_scaling_factor = 0.0;
    double sum_crossover_rate = 0.0;
    next_mu_scaling_factor = 0.0;
    next_mu_crossover_rate = 0.0;
    for (int i = 0; i < success_cnt; ++i) {
      sum += fitness_differences[i];
    }
    for (int i = 0; i < success_cnt; ++i) {
      next_mu_scaling_factor += (fitness_differences[i] / sum) * success_scaling_factors[i] * success_scaling_factors[i];
      sum_scaling_factor += (fitness_differences[i] / sum) * success_scaling_factors[i];
      next_mu_crossover_rate += (fitness_differences[i] / sum) * success_crossover_rates[i] * success_crossover_rates[i];
      sum_crossover_rate += (fitness_differences[i] / sum) * success_crossover_rates[i];
    }
    next_mu_scaling_factor /= sum_scaling_factor;
    if ((-1.0 == mu_crossover_rate) || (0.0 == sum_crossover_rate)) {
      next_mu_crossover_rate = -1.0;
    }
    else {
      next_mu_crossover_rate /= sum_crossover_rate;
    }
  }
  free(success_scaling_factors);
  free(success_crossover_rates);
  free(fitness_differences);
  SHADE_individual * const sorted_next_population = get_sorted_population(next_population, population_size, problem_size);
  terminate_population(next_population, population_size);
  return (SHADE_selection_results) { sorted_next_population, next_archive, next_mu_scaling_factor, next_mu_crossover_rate, next_archive_cnt };
}

static int get_best_index(const SHADE_individual * const population, const int population_size) {
  int best_index = 0;
  for (int i = 1; i < population_size; ++i) {
    if (population[i].fitness < population[best_index].fitness) {
      best_index = i;
    }
  }
  return best_index;
}
