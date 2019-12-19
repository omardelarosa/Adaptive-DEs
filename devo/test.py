import ctypes as c
import devo.DE
import devo.LSHADE
import devo.SHADE
import devo.jDE
import devo.JADE
import devo.CoDE
import numpy as np
from scipy.optimize import rosen

VERBOSE_LOGGING = False
NUM_ITERATIONS = 150000


def sphere_function(vec, dimension):
    result = 0.0
    for i in range(dimension):
        x = vec[i]
        result += (x * x)
    return c.c_double(result)


def rosenbrock(vec, dimension):
    np_vec = np.zeros((dimension,), dtype='float32')
    for i in range(0, dimension):
        np_vec[i] = vec[i]
    res = rosen(np_vec)
    return c.c_double(res)


class Optimizer():
    def __init__(self, population_size):
        _min = -100.0  # min
        _max = 100.0  # max
        dimension = 30
        self.init_population = (
            _max - _min) * np.random.uniform(size=(population_size, dimension)) + _min
        self.init_fitnesses = (_max - _min) * \
            np.random.uniform(size=population_size) + _min
        self.out_population = np.zeros(
            (population_size, dimension), dtype='float32')
        self.out_fitnesses = np.zeros((population_size, 1), dtype='float32')
        self.cached = 0.0
        return

    def update_result(self, result):
        self.cached = result
        return

    def NULL_CALLBACK(self, x, y, z, w):
        return None

    def results_callback(self, population, fitness_values, population_size, problem_size):
        # Store results to python memory containers
        # Store population
        for i in range(0, population_size * problem_size):
            row = int(i / problem_size)
            col = i % problem_size
            self.out_population[row][col] = population[i]

        # Store fitness values
        for j in range(0, population_size):
            f = fitness_values[j]
            self.out_fitnesses[j] = f
        return


def test(name, module, num_iterations, population_size, obj_f):
    opt = Optimizer(population_size)
    f = obj_f
    result_01 = module.run(
        num_iterations,
        population_size,
        0.5,
        0.9,
        f,
        30,
        -100.0,
        100.0,
        None,
        None,
        opt.NULL_CALLBACK
    )

    print((name + ": "), result_01)

    result_02 = module.run(
        num_iterations,
        population_size,
        0.5,
        0.9,
        f,
        30,
        -100.0,
        100.0,
        opt.init_population.ctypes.data_as(c.POINTER(c.c_double)),
        opt.init_fitnesses.ctypes.data_as(c.POINTER(c.c_double)),
        opt.results_callback
    )
    # print(opt.out_fitnesses)
    min_fit_idx = opt.out_fitnesses.tolist().index(opt.out_fitnesses.min())
    min_individual = opt.out_population[min_fit_idx]
    obj_result = f(
        min_individual, 30).value

    print(name + " (with provided population): ",
          obj_result, "idx: ", min_fit_idx)

    if VERBOSE_LOGGING:
        i = 0
        for p in opt.out_population:
            if i >= len(opt.out_fitnesses):
                break
            print(i, ": expected:", opt.out_fitnesses[i],
                  " actual: ", f(p, 30).value)
            i += 1


def run_all():
    population_size = 100
    problem_size = 30
    objective_functions = {
        "Rosenbrock": rosenbrock,
        "Sphere": sphere_function
    }
    for n, f in objective_functions.items():
        print("----- Testing: ", n)
        test("DE", devo.DE, NUM_ITERATIONS, population_size, f)
        test("LSHADE", devo.LSHADE, NUM_ITERATIONS, problem_size * 18, f)
        test("SHADE", devo.SHADE, NUM_ITERATIONS, population_size, f)
        test("jDE", devo.jDE, NUM_ITERATIONS, population_size, f)
        test("JADE", devo.JADE, NUM_ITERATIONS, population_size, f)
        test("CoDE", devo.CoDE, NUM_ITERATIONS, problem_size, f)
