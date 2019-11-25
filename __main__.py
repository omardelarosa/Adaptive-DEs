import ctypes as c
import DE
import numpy as np


def sphere_function(vec, dimension):
    result = 0.0
    for i in range(0, dimension):
        result = vec[i] * vec[i]
    return c.c_double(result)


def run_all():
    result_01 = DE.run_DE(
        150000,
        100,
        0.5,
        0.9,
        sphere_function,
        30,
        -100.0,
        100.0
    )

    print("DE: ", result_01)

    init_population = np.random.rand(100, 30)
    init_fitnesses = np.random.rand(100, 1)
    out_population = np.zeros((100, 30))
    out_fitnesses = np.zeros((100, 1))

    result_02 = DE.run_DE_with_population_provided(
        150000,
        100,
        0.5,
        0.9,
        sphere_function,
        30,
        -100.0,
        100.0,
        init_population.ctypes.data_as(c.POINTER(c.c_double)),
        init_fitnesses.ctypes.data_as(c.POINTER(c.c_double)),
        out_population.ctypes.data_as(c.POINTER(c.c_double)),
        out_fitnesses.ctypes.data_as(c.POINTER(c.c_double))
    )

    print("DE (with provided population)")
    print("Initial Population & Fitness")
    print(init_population)
    print(init_fitnesses)

    print("Output Population & Fitness")
    print(out_population)
    print(out_fitnesses)


run_all()
