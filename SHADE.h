// SHADE.h
#ifndef SHADE_h

typedef struct {
  double *x;
  double fitness;
  double scaling_factor;
  double crossover_rate;
} SHADE_individual;

typedef struct {
  double scaling_factor;
  double crossover_rate;
} SHADE_parameter_control_results;

typedef struct {
  SHADE_individual *population;
  SHADE_individual *archive;
  double mu_scaling_factor;
  double mu_crossover_rate;
  int archive_cnt;
} SHADE_selection_results;

double run_SHADE(const int max_function_evaluations, const int population_size,
                 const double (*objective_function)(const double *const,
                                                    const int),
                 const int problem_size, const double lower_bound,
                 const double upper_bound, double *initial_population,
                 double *fitness_values,
                 void (*results_callback)(const double *population_results,
                                          const double *fitness_results,
                                          const int population_size,
                                          const int problem_size));

#endif
