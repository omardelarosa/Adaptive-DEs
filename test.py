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


class Optimizer():
    def __init__(self, population_size):
        self.init_population = np.random.rand(population_size, 30)
        self.init_fitnesses = np.random.rand(population_size, 1)
        self.out_population = np.zeros((population_size, 30), dtype='float32')
        self.out_fitnesses = np.zeros((population_size, 1), dtype='float32')
        self.c = 0.0
        return

    def update_result(self, result):
        self.c = result
        return

    def NULL_CALLBACK(self, x, y, z, w):
        return None

    def sphere_function(self, vec, dimension):
        result = 0.0
        for i in range(dimension):
            x = vec[i]
            result += (x * x)
        self.update_result(result)
        return c.c_double(self.c)

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


def test(name, module, num_iterations, population_size):

    opt = Optimizer(population_size)

    result_01 = module.run(
        num_iterations,
        population_size,
        0.5,
        0.9,
        opt.sphere_function,
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
        opt.sphere_function,
        30,
        -100.0,
        100.0,
        opt.init_population.ctypes.data_as(c.POINTER(c.c_double)),
        opt.init_fitnesses.ctypes.data_as(c.POINTER(c.c_double)),
        opt.results_callback
    )

    min_fit_idx = opt.out_fitnesses.tolist().index(opt.out_fitnesses.min())
    min_individual = opt.out_population[min_fit_idx]
    obj_result = opt.sphere_function(
        min_individual, 30).value

    print(name + " (with provided population): ",
          obj_result, "idx: ", min_fit_idx)

    if VERBOSE_LOGGING:
        i = 0
        for p in opt.out_population:
            if i >= len(opt.out_fitnesses):
                break
            print(i, ": expected:", opt.out_fitnesses[i],
                  " actual: ", opt.sphere_function(p, 30).value, p)
            i += 1


def run_all():
    # test("DE", devo.DE, NUM_ITERATIONS, 100)
    # test("LSHADE", devo.LSHADE, NUM_ITERATIONS, 30 * 18)
    # test("SHADE", devo.SHADE, NUM_ITERATIONS, 100)
    test("jDE", devo.jDE, NUM_ITERATIONS, 100)
    # test("JADE", devo.JADE, NUM_ITERATIONS, 100)
    # test("CoDE", devo.CoDE, NUM_ITERATIONS, 30)


run_all()
