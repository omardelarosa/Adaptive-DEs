import ctypes as c
import devo.DE
import numpy as np


def sphere_function(vec, dimension):
    result = 0.0
    for i in range(0, dimension):
        result += (vec[i] * vec[i])
    return c.c_double(result)


def run_all():
    result_01 = devo.DE.run_DE(
        150000,
        100,
        0.5,
        0.9,
        sphere_function,
        30,
        -100.0,
        100.0,
        None,
        None,
        lambda x: x
    )

    print("DE: ", result_01)

    init_population = np.random.rand(100, 30)
    init_fitnesses = np.random.rand(100, 1)
    out_population = np.zeros((100, 30), dtype='float32')
    out_fitnesses = np.zeros((100, 1), dtype='float32')

    def results_callback(population, fitness_values, population_size, problem_size):
        # Store results to python memory containers

        # Store population
        for i in range(0, population_size * problem_size):
            row = int(i / problem_size)
            col = i % problem_size
            out_population[row][col] = population[i]

        # Store fitness values
        for j in range(0, population_size):
            out_fitnesses = fitness_values[j]
        return

    result_02 = devo.DE.run_DE(
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
        results_callback
    )

    print("DE (with provided population)")
    print("Initial Population & Fitness")

    print("Resulting Population", out_population)
    print("Resulting Fitnesses", out_fitnesses)
    min_fit_idx = out_fitnesses.tolist().index(out_fitnesses.min())
    print("MinFitness: ", out_fitnesses[min_fit_idx], "Idx: ", min_fit_idx)
    print("MinIndividual: ", out_population[min_fit_idx])
    print("TestObjectiveFunction:", sphere_function(
        out_population[min_fit_idx], 30))


run_all()
