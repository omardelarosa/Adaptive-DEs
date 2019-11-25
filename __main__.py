import ctypes as c
import time as time

OBJFUNC = c.CFUNCTYPE(None, c.POINTER(c.c_double), c.c_int)


def sphere_function(vec, dimension):
    result = 0.0
    for i in range(0, dimension):
        result = vec[i] * vec[i]
    return c.c_double(result)


c_sphere_func_func = OBJFUNC(sphere_function)

DE = c.CDLL("dist/DE.so")
# DE.run_DE_with_population_provided.argtypes = (
#     c.c_int,  # max_function_evaluations
#     c.c_int,  # population_size
#     c.c_double,  # scaling_factor
#     c.c_double,  # crossover_rate
#     OBJFUNC,  # objective_function
#     c.c_int,  # problem_size
#     c.c_double,  # lower_bound
#     c.c_double,  # upper_bound
#     c.POINTER(c.POINTER)  # initial_population
# )

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


def run_all(seed):
    global _devo
    s = c.c_uint(seed)
    result = DE.run_DE(
        c.c_int(150000),
        c.c_int(100),
        c.c_double(0.5),
        c.c_double(0.9),
        c_sphere_func_func,
        c.c_int(30),
        c.c_double(-100.0),
        c.c_double(100.0)
    )
    return result


print(run_all(1574708500))
