# Genetic Algorithm for LCVRP

C++ implementation of a **genetic algorithm** for solving a grouped version of the  
**Length-Constrained Capacitated Vehicle Routing Problem (LCVRP)**.

The program loads a problem instance, evaluates candidate solutions and applies evolutionary operators to search for a low-cost grouping of customers.

This project was created as part of a university **optimization / algorithms course**.

---

## Features

- Loading `.lcvrp` problem instances
- Genetic algorithm optimization
- Tournament selection
- One-point crossover
- Mutation operator
- Fitness evaluation based on route cost
- Best solution tracking during the run
- Configurable algorithm parameters

---

## Project Structure

src/
├── main.cpp
├── CEvaluator.cpp
├── CEvaluator.h
├── CGeneticAlgorithm.cpp
├── CGeneticAlgorithm.h
├── CIndividual.cpp
├── CIndividual.h
├── ProblemData.cpp
├── ProblemData.h
└── ProblemLoader.cpp

data/
└── A-n32-k5.lcvrp

---

## How It Works

1. The program loads an `.lcvrp` instance.
2. The evaluator prepares distance and demand data.
3. A random population of individuals is generated.
4. Each individual represents customer-to-group assignments.
5. The algorithm iteratively performs:
   - selection
   - crossover
   - mutation
   - fitness evaluation
6. The best solution found is printed after the simulation.

---

## Running the Program

Compile:
g++ src/*.cpp -o genetic-algorithm

Run:
./genetic-algorithm data/A-n32-k5.lcvrp 5

Optional parameters:
<population_size> <crossover_probability> <mutation_probability> <iterations> <seed>

Example:
./genetic-algorithm data/A-n32-k5.lcvrp 5 100 0.6 0.1 200 42

---

## Technologies

- C++
- Standard Library
- Visual Studio

---

## Author

University project for **optimization and evolutionary algorithms**.
