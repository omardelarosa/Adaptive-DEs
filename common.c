// common.c
#include "common.h"

double *run_rand_1_bin(const double * const target, const double * const donor1, const double * const donor2, const double * const donor3,
		       const double scaling_factor, const double crossover_rate, const int dimension, const double lower_bound, const double upper_bound) {
  double * const x = (double *)malloc(sizeof(double) * dimension);
  const int j_rand = rand() % dimension;
  for (int j = 0; j < dimension; ++j) {
    if (((rand() / (double)RAND_MAX) <= crossover_rate) || (j_rand == j)) {
      x[j] = donor1[j] + scaling_factor * (donor2[j] - donor3[j]);
      if (lower_bound > x[j]) x[j] = (target[j] + lower_bound) / 2.0;
      if (upper_bound < x[j]) x[j] = (upper_bound + target[j]) / 2.0;
    }
    else {
      x[j] = target[j];
    }
  }
  return x;
}

double *run_rand_2_bin(const double * const target, const double * const donor1, const double * const donor2, const double * const donor3,
		       const double * const donor4, const double * const donor5, const double scaling_factor, const double crossover_rate,
		       const int dimension, const double lower_bound, const double upper_bound) {
  double * const x = (double *)malloc(sizeof(double) * dimension);
  const int j_rand = rand() % dimension;
  for (int j = 0; j < dimension; ++j) {
    if (((rand() / (double)RAND_MAX) <= crossover_rate) || (j_rand == j)) {
      x[j] = donor1[j] + scaling_factor * (donor2[j] - donor3[j]) + scaling_factor * (donor4[j] - donor5[j]);
      if (lower_bound > x[j]) x[j] = (target[j] + lower_bound) / 2.0;
      if (upper_bound < x[j]) x[j] = (upper_bound + target[j]) / 2.0;
    }
    else {
      x[j] = target[j];
    }
  }
  return x;
}

double *run_current_to_rand_1(const double * const target, const double * const donor1, const double * const donor2, const double * const donor3,
			      const double scaling_factor, const int dimension, const double lower_bound, const double upper_bound) {
  double * const x = (double *)malloc(sizeof(double) * dimension);
  const double k = rand() / (double)RAND_MAX;
  for (int j = 0; j < dimension; ++j) {
    x[j] = target[j] + k * (donor1[j] - target[j]) + scaling_factor * (donor2[j] - donor3[j]);
    if (lower_bound > x[j]) x[j] = (target[j] + lower_bound) / 2.0;
    if (upper_bound < x[j]) x[j] = (upper_bound + target[j]) / 2.0;
  }
  return x;
}

double *run_target_to_best_1_bin(const double * const target, const double * const best, const double * const donor1, const double * const donor2,
				 const double scaling_factor, const double crossover_rate, const int dimension, const double lower_bound, const double upper_bound) {
  double * const x = (double *)malloc(sizeof(double) * dimension);
  const int j_rand = rand() % dimension;
  for (int j = 0; j < dimension; ++j) {
    if (((rand() / (double)RAND_MAX) <= crossover_rate) || (j_rand == j)) {
      x[j] = target[j] + scaling_factor * (best[j] - target[j]) + scaling_factor * (donor1[j] - donor2[j]);
      if (lower_bound > x[j]) x[j] = (target[j] + lower_bound) / 2.0;
      if (upper_bound < x[j]) x[j] = (upper_bound + target[j]) / 2.0;
    }
    else {
      x[j] = target[j];
    }
  }
  return x;
}

double generate_Gaussian(const double location, const double scale) {
  // Marsaglia polar method
  // https://en.wikipedia.org/wiki/Marsaglia_polar_method
  double u, v, s;
  do {
    u = (rand() / (double)RAND_MAX) * 2.0 - 1.0;
    v = (rand() / (double)RAND_MAX) * 2.0 - 1.0;
    s = u * u + v * v;
  } while (0.0 == s || 1.0 <= s);
  s = sqrt(-2.0 * log(s) / s);
  return location + scale * u * s;
}

double generate_Cauchy(const double location, const double scale) {
  return location + scale * tan(M_PI * ((rand() / (double)RAND_MAX) - 0.5));
}

double arithmetic_mean(const double *a, const int size) {
  double t = 0.0;
  for (int i = 0; i < size; ++i) {
    t += a[i];
  }
  return t / (double)size;
}

double Lehmer_mean(const double *a, const int size) {
  double t1 = 0.0;
  double t2 = 0.0;
  for (int i = 0; i < size; ++i) {
    t1 += a[i] * a[i];
    t2 += a[i];
  }
  return t1 / t2;
}

double weighted_arithmetic_mean(const double *a, const double *w, const int size) {
  double t = 0.0;
  for (int i = 0; i < size; ++i) {
    t += w[i] * a[i];
  }
  return t;
}

double weighted_Lehmer_mean(const double *a, const double *w, const int size) {
  double t1 = 0.0;
  double t2 = 0.0;
  for (int i = 0; i < size; ++i) {
    t1 += w[i] * a[i] * a[i];
    t2 += w[i] * a[i];
  }
  return t1 / t2;
}