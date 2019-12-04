import ctypes as c
import devo.DE
import devo.LSHADE
import devo.SHADE
import devo.jDE
import devo.JADE
import devo.CoDE
import numpy as np

VERBOSE_LOGGING = True
NUM_ITERATIONS = 150000


def test(name, module, num_iterations, population_size):
    init_population = np.random.rand(population_size, 30)
    init_fitnesses = np.random.rand(population_size, 1)
    out_population = np.zeros((population_size, 30), dtype='float32')
    out_fitnesses = np.zeros((population_size, 1), dtype='float32')

    def NULL_CALLBACK(x, y, z, w):
        return None

    def sphere_function(vec, dimension):
        result = 0.0
        for i in range(dimension):
            x = vec[i]
            result += (x * x)
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
            f = fitness_values[j]
            out_fitnesses[j] = f
        return

    result_01 = module.run(
        num_iterations,
        population_size,
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
        num_iterations,
        population_size,
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

    min_fit_idx = out_fitnesses.tolist().index(out_fitnesses.min())
    min_individual = out_population[min_fit_idx]
    obj_result = sphere_function(
        min_individual, 30).value

    print(name + " (with provided population): ",
          obj_result, "idx: ", min_fit_idx)

    if VERBOSE_LOGGING:
        i = 0
        for p in out_population:
            if i >= len(out_fitnesses):
                break
            print(i, ": expected:", out_fitnesses[i],
                  " actual: ", sphere_function(p, 30).value, p)
            i += 1


def run_all():
    # test("DE", devo.DE, NUM_ITERATIONS, 100)
    # test("LSHADE", devo.LSHADE, NUM_ITERATIONS, 30 * 18)
    # test("SHADE", devo.SHADE, NUM_ITERATIONS, 100)
    # test("jDE", devo.jDE, NUM_ITERATIONS, 100)
    test("JADE", devo.JADE, NUM_ITERATIONS, 100)
    # test("CoDE", devo.CoDE, NUM_ITERATIONS, 30)


run_all()
