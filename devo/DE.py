import ctypes as c
import time as time
import os
import glob

_MODULE_PATH = os.path.dirname(__file__)

# Module build directory
_BUILD_PATH = "build"

# # DLL path
_DE_SO_PATH = 'DE.*.so'
# # the absolute path to the C shared object library
_LIB_PATH = os.path.join(_MODULE_PATH, "..",
                         _BUILD_PATH, "**", _DE_SO_PATH)
print("LIB_PATH", _LIB_PATH)
_DE = None

try:
    _DE = c.cdll.LoadLibrary(glob.glob(_LIB_PATH)[0])
except IndexError:
    raise OSError('missing static DE*.so library!')


# Objective function
OBJFUNC = c.CFUNCTYPE(None, c.POINTER(c.c_double), c.c_int)

# Type annotation for result function
RESULTFUNC = c.CFUNCTYPE(None, c.POINTER(c.c_double),
                         c.POINTER(c.c_double), c.c_int, c.c_int)

_DE.run_DE.argtypes = (
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
    RESULTFUNC  # a callback to handle results
)

_DE.run_DE.restype = c.c_double


def run(max_function_evaluations, population_size, scaling_factor, crossover_rate, objective_function, problem_size, lower_bound, upper_bound, init_population, init_fitnesses, result_callback):
    result = _DE.run_DE(
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
        RESULTFUNC(result_callback)
    )
    return result
