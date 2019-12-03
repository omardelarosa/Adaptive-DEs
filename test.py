import ctypes as c
import devo.DE
import devo.LSHADE
import devo.SHADE
import numpy as np

VERBOSE_LOGGING = False

init_population = np.random.rand(100, 30)
init_fitnesses = np.random.rand(100, 1)
out_population = np.zeros((100, 30), dtype='float32')
out_fitnesses = np.zeros((100, 1), dtype='float32')


def NULL_CALLBACK(x, y, z, w): return None


def reset():
    init_population = np.random.rand(100, 30)
    init_fitnesses = np.random.rand(100, 1)
    out_population = np.zeros((100, 30), dtype='float32')
    out_fitnesses = np.zeros((100, 1), dtype='float32')


def sphere_function(vec, dimension):
    result = 0.0
    for i in range(0, dimension):
        result += (vec[i] * vec[i])
    return c.c_double(result)


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


def test_DE():
    reset()
    result_01 = devo.DE.run(
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
        NULL_CALLBACK
    )

    print("DE: ", result_01)

    result_02 = devo.DE.run(
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

    min_fit_idx = out_fitnesses.tolist().index(out_fitnesses.min())

    if VERBOSE_LOGGING:
        print("Resulting Population", out_population)
        print("Resulting Fitnesses", out_fitnesses)
    print("TestObjectiveFunction:", sphere_function(
        out_population[min_fit_idx], 30).value)
    print("MinFitness: ", out_fitnesses[min_fit_idx], "Idx: ", min_fit_idx)
    print("MinIndividual: ", out_population[min_fit_idx])


def test_LSHADE():
    reset()
    result_01 = devo.LSHADE.run(
        150000,
        sphere_function,
        30,
        -100.0,
        100.0,
        None,
        None,
        NULL_CALLBACK
    )

    print("LSHADE: ", result_01)

    result_02 = devo.LSHADE.run(
        150000,
        sphere_function,
        30,
        -100.0,
        100.0,
        init_population.ctypes.data_as(c.POINTER(c.c_double)),
        init_fitnesses.ctypes.data_as(c.POINTER(c.c_double)),
        results_callback
    )

    print("LSHADE (with provided population)")
    print("Initial Population & Fitness")

    min_fit_idx = out_fitnesses.tolist().index(out_fitnesses.min())

    if VERBOSE_LOGGING:
        print("Resulting Population", out_population)
        print("Resulting Fitnesses", out_fitnesses)
    print("TestObjectiveFunction:", sphere_function(
        out_population[min_fit_idx], 30).value)
    print("MinFitness: ", out_fitnesses[min_fit_idx], "Idx: ", min_fit_idx)
    print("MinIndividual: ", out_population[min_fit_idx])


def test_SHADE():
    reset()
    result_01 = devo.SHADE.run(
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
        NULL_CALLBACK
    )

    print("SHADE: ", result_01)

    result_02 = devo.SHADE.run(
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

    print("SHADE (with provided population)")
    print("Initial Population & Fitness")

    min_fit_idx = out_fitnesses.tolist().index(out_fitnesses.min())

    if VERBOSE_LOGGING:
        print("Resulting Population", out_population)
        print("Resulting Fitnesses", out_fitnesses)
    print("TestObjectiveFunction:", sphere_function(
        out_population[min_fit_idx], 30).value)
    print("MinFitness: ", out_fitnesses[min_fit_idx], "Idx: ", min_fit_idx)
    print("MinIndividual: ", out_population[min_fit_idx])


def run_all():
    test_DE()
    test_LSHADE()
    test_SHADE()


run_all()
