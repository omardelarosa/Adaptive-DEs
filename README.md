# Adaptive DEs

Adaptive Differential Evolution tools.

## Build C Libraries

```
$ make build build-lib
```

## Usage

After building, run `dist/devo` executable

```
./dist/devo
```

## Python Bindings

### Installation

```
git clone https://github.com/gzjj05/Adaptive-DEs.git
cd Adaptive-DEs
pip install -e .
```

### Usage

Using with numpy

```python
import devo.DE as DE

import ctypes as c
import numpy as np

# Initialize variables, data containers
init_population = np.random.rand(100, 30)
init_fitnesses = np.random.rand(100, 1)
out_population = np.zeros((100, 30), dtype='float32')
out_fitnesses = np.zeros((100, 1), dtype='float32')

# A sample Objective Function
def sphere_function(vec, dimension):
    result = 0.0
    for i in range(0, dimension):
        result = vec[i] * vec[i]
    return c.c_double(result)

# Callback to handle the results
def results_callback(population, fitness_values, population_size, problem_size):
    # Store results to python memory containers

    # Store population
    for i in range(0, population_size * problem_size):
        row = int(i / problem_size)
        col = i % problem_size
        out_population[row][col] = population[i]
    return

    # Store fitness values
    for j in range(0, population_size):
        out_fitnesses = fitness_values[j]
    return

result = DE.run_DE_with_population_provided(
    150000,# max_function_evaluations
    100,# population_size
    0.5, # scaling_factor
    0.9, # crossover_rate
    sphere_function, # objective_function
    30, # problem_size
    -100.0, # lower_bound
    100.0, # upper_bound
    init_population.ctypes.data_as(c.POINTER(c.c_double)), # initial_population as 2-D array pointer
    init_fitnesses.ctypes.data_as(c.POINTER(c.c_double)), # initial fitness values as 1-D array pointer
    results_callback
)

print("DE (with provided population)")
print("Initial Population & Fitness")
print(init_population)
print(init_fitnesses)
print("Output")
print(out_population)
print(out_fitnesses)
```
