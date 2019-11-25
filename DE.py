import ctypes as c
import time as time

OBJFUNC = c.CFUNCTYPE(None, c.POINTER(c.c_double), c.c_int)

DE = c.CDLL("dist/DE.so")

DE.run_DE_with_population_provided.argtypes = (
    c.c_int,  # max_function_evaluations
    c.c_int,  # population_size
    c.c_double,  # scaling_factor
    c.c_double,  # crossover_rate
    OBJFUNC,  # objective_function
    c.c_int,  # problem_size
    c.c_double,  # lower_bound
    c.c_double,  # upper_bound
    c.POINTER(c.c_double),  # initial_population
    c.POINTER(c.c_double),  # fitness_values
    c.POINTER(c.c_double),  # population_results
    c.POINTER(c.c_double)  # fitness value results
)

DE.run_DE.argtypes = (
    c.c_int,  # max_function_evaluations
    c.c_int,  # population_size
    c.c_double,  # scaling_factor
    c.c_double,  # crossover_rate
    OBJFUNC,  # objective_function
    c.c_int,  # problem_size
    c.c_double,  # lower_bound
    c.c_double  # upper_bound
)


def run_DE(max_function_evaluations, population_size, scaling_factor, crossover_rate, objective_function, problem_size, lower_bound, upper_bound):
    result = DE.run_DE(
        c.c_int(max_function_evaluations),
        c.c_int(population_size),
        c.c_double(scaling_factor),
        c.c_double(crossover_rate),
        OBJFUNC(objective_function),
        c.c_int(30),
        c.c_double(-100.0),
        c.c_double(100.0)
    )
    return result


def run_DE_with_population_provided(max_function_evaluations, population_size, scaling_factor, crossover_rate, objective_function, problem_size, lower_bound, upper_bound, init_population, init_fitnesses, out_population, out_fitnesses):
    result = DE.run_DE(
        c.c_int(max_function_evaluations),
        c.c_int(population_size),
        c.c_double(scaling_factor),
        c.c_double(crossover_rate),
        OBJFUNC(objective_function),
        c.c_int(30),
        c.c_double(-100.0),
        c.c_double(100.0),
        init_population,
        init_fitnesses,
        out_population,
        out_fitnesses
    )
    return result
