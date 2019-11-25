import ctypes as c
import time as time
# import os
# import glob

# Module directory
# _MODULE_PATH = os.path.dirname(__file__)

# # DLL path
# _SO_PATH = 'DE*.so'
# # the absolute path to the C shared object library
# _LIB_PATH = os.path.join(_MODULE_PATH, _SO_PATH)

# _DE = None

# try:
#     _DE = c.cdll.LoadLibrary(glob.glob(_LIB_PATH)[0])
# except IndexError:
#     raise OSError('missing static DE*.so library!')

_DE = c.cdll.LoadLibrary("dist/DE.so")


OBJFUNC = c.CFUNCTYPE(None, c.POINTER(c.c_double), c.c_int)


_DE.run_DE_with_population_provided.argtypes = (
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

_DE.run_DE.argtypes = (
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
    result = _DE.run_DE(
        max_function_evaluations,
        population_size,
        scaling_factor,
        crossover_rate,
        OBJFUNC(objective_function),
        problem_size,
        lower_bound,
        upper_bound
    )
    return result


def run_DE_with_population_provided(max_function_evaluations, population_size, scaling_factor, crossover_rate, objective_function, problem_size, lower_bound, upper_bound, init_population, init_fitnesses, out_population, out_fitnesses):
    result = _DE.run_DE_with_population_provided(
        c.c_int(max_function_evaluations),
        c.c_int(population_size),
        c.c_double(scaling_factor),
        c.c_double(crossover_rate),
        OBJFUNC(objective_function),
        problem_size,
        lower_bound,
        upper_bound,
        init_population,
        init_fitnesses,
        out_population,
        out_fitnesses
    )
    return result


__all__ = []
