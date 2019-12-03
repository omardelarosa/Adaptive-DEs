// CoDE.h
#ifndef CoDE_h

typedef struct {
  double *x;
  double fitness;
} CoDE_individual;

double run_CoDE(const int max_function_evaluations, const int population_size,
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
