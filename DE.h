// DE.h
#ifndef DE_h

typedef struct {
  double *x;
  double fitness;
} DE_individual;

double run_DE(const int max_function_evaluations, const int population_size,
              const double scaling_factor, const double crossover_rate,
              const double (*objective_function)(const double *const,
                                                 const int),
              const int problem_size, const double lower_bound,
              const double upper_bound);

double run_DE_with_population_provided(
    const int max_function_evaluations, const int population_size,
    const double scaling_factor, const double crossover_rate,
    const double (*objective_function)(const double *const, const int),
    const int problem_size, const double lower_bound, const double upper_bound,
    double *initial_population, double *fitness_values,
    double *population_results, double *fitness_results);

#endif
