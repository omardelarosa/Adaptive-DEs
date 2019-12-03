// JADE.h
#ifndef JADE_h

typedef struct {
  double *x;
  double fitness;
  double scaling_factor;
  double crossover_rate;
} JADE_individual;

typedef struct {
  double scaling_factor;
  double crossover_rate;
} JADE_parameter_control_results;

typedef struct {
  JADE_individual *population;
  JADE_individual *archive;
  double mu_scaling_factor;
  double mu_crossover_rate;
  int archive_cnt;
} JADE_selection_results;

double run_JADE(const int max_function_evaluations, const int population_size,
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
