// jDE.h
#ifndef jDE_h

typedef struct {
  double *x;
  double fitness;
  double scaling_factor;
  double crossover_rate;
} jDE_individual;

typedef struct {
  double scaling_factor;
  double crossover_rate;
} jDE_parameter_control_results;

double run_jDE(const int max_function_evaluations, const int population_size,
	       const double(*objective_function)(const double * const, const int), const int problem_size, const double lower_bound, const double upper_bound);

#endif
