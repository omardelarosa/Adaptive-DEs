import ctypes as c
import devo.DE
import devo.LSHADE
import devo.SHADE
import devo.jDE
import devo.JADE
import devo.CoDE
import numpy as np

VERBOSE_LOGGING = False

init_population = np.random.rand(100, 30)
init_fitnesses = np.random.rand(100, 1)
out_population = np.zeros((100, 30), dtype='float32')
out_fitnesses = np.zeros((100, 1), dtype='float32')


def NULL_CALLBACK(x, y, z, w):
    return None


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


def test(name, module):
    reset()
    result_01 = module.run(
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

    print((name + ": "), result_01)

    result_02 = module.run(
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

    print(name + " (with provided population)")
    print("Initial Population & Fitness")

    min_fit_idx = out_fitnesses.tolist().index(out_fitnesses.min())

    if VERBOSE_LOGGING:
        print("Resulting Population", out_population)
        print("Resulting Fitnesses", out_fitnesses)
    print("TestObjectiveFunction:", sphere_function(
        out_population[min_fit_idx], 30).value)
    print("MinFitness: ", out_fitnesses[min_fit_idx], "Idx: ", min_fit_idx)
    print("MinIndividual: ", out_population[min_fit_idx])


# NOTE: This function has a different call signature
def test_LSHADE():
    module = devo.LSHADE
    reset()
    result_01 = module.run(
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

    result_02 = module.run(
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


def run_all():
    test("DE", devo.DE)
    # test("LSHADE", devo.LSHADE)
    test_LSHADE()
    test("SHADE", devo.SHADE)
    test("jDE", devo.jDE)
    test("JADE", devo.JADE)
    test("CoDE", devo.CoDE)


run_all()
