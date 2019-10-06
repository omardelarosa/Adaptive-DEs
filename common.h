// common.h
#ifndef common_h
#include <math.h>
#include <stdlib.h>
#include <string.h>

double *run_rand_1_bin(const double * const target, const double * const donor1, const double * const donor2, const double * const donor3,
		       const double scaling_factor, const double crossover_rate, const int dimension, const double lower_bound, const double upper_bound);
double *run_rand_2_bin(const double * const target, const double * const donor1, const double * const donor2, const double * const donor3,
		       const double * const donor4, const double * const donor5, const double scaling_factor, const double crossover_rate,
		       const int dimension, const double lower_bound, const double upper_bound);
double *run_current_to_rand_1(const double * const target, const double * const donor1, const double * const donor2, const double * const donor3,
			      const double scaling_factor, const int dimension, const double lower_bound, const double upper_bound);
double *run_target_to_best_1_bin(const double * const target, const double * const best, const double * const donor1, const double * const donor2,
				 const double scaling_factor, const double crossover_rate, const int dimension, const double lower_bound, const double upper_bound);

double generate_Gaussian(const double location, const double scale);
double generate_Cauchy(const double location, const double scale);
double arithmetic_mean(const double *a, const int size);
double Lehmer_mean(const double *a, const int size);
double weighted_arithmetic_mean(const double *a, const double *w, const int size);
double weighted_Lehmer_mean(const double *a, const double *w, const int size);

#endif
