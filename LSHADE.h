// LSHADE.h
#ifndef LSHADE_h

typedef struct {
  double *x;
  double fitness;
  double scaling_factor;
  double crossover_rate;
} LSHADE_individual;

typedef struct {
  double scaling_factor;
  double crossover_rate;
} LSHADE_parameter_control_results;

typedef struct {
  LSHADE_individual *population;
  LSHADE_individual *archive;
  double mu_scaling_factor;
  double mu_crossover_rate;
  int population_size;
  int archive_size;
  int archive_cnt;
} LSHADE_selection_results;

double run_LSHADE(const int max_function_evaluations,
		  const double(*objective_function)(const double * const, const int), const int problem_size, const double lower_bound, const double upper_bound);

#endif
