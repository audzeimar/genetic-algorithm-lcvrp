# Genetic Algorithm for LCVRP

A C++ project implementing a **genetic algorithm** for solving a grouped version of the **Length-Constrained Capacitated Vehicle Routing Problem (LCVRP)**.

The program loads a problem instance from a `.lcvrp` file, evaluates candidate solutions, and applies a genetic algorithm with selection, crossover, and mutation to search for a low-cost grouping of customers.

This project was created as part of a university programming / optimization course.

---

## Project Overview

The solution is based on the following components:

- **ProblemLoader** – reads problem instances from `.lcvrp` files
- **ProblemData** – stores instance data such as coordinates, demands, distances, and permutation
- **CEvaluator** – validates instances and computes route cost / fitness
- **CIndividual** – represents a single candidate solution (genotype)
- **CGeneticAlgorithm** – manages the population and genetic operators
- **main.cpp** – runs the simulation and prints results

---

## Features

- Loading `.lcvrp` problem instances from file
- Support for:
  - node coordinates
  - explicit distance matrices
  - vehicle capacity constraints
  - route length constraints
- Evaluation of grouped customer assignments
- Genetic algorithm with:
  - random population initialization
  - tournament selection
  - one-point crossover
  - mutation
  - best-so-far tracking
- Console-based execution with configurable parameters

---

## Project Structure

```text
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
├── ProblemLoader.cpp
└── ProblemLoader.h

data/
└── A-n32-k5.lcvrp

---

## How It Works

1. The program loads an .lcvrp instance file.
2. The evaluator validates the instance and prepares distance data.
3. The genetic algorithm initializes a random population.
4. Each individual encodes customer-to-group assignments.
5. The algorithm iteratively applies:
    tournament selection
    one-point crossover
    mutation
    fitness evaluation
6. The best individual found during the run is printed at the end.

---

## Input Parameters

The program accepts the following command-line arguments:
<instance_path> <num_groups> [population_size] [crossover_probability] [mutation_probability] [iterations] [seed]

Example:
genetic-algorithm-lcvrp data/A-n32-k5.lcvrp 5 100 0.6 0.1 200 42

Example Output
Instancja: A-n32-k5
Wymiary (wezly): 32  (klienci: 31)
Grupy: 5
Iteracje GA: 200

Najlepszy osobnik (fitness = 1/(1+cost)):
  fitness = ...
  cost    = ...

---

## Technologies

C++
Visual Studio
Standard C++ library
Learning Objectives

---

## This project demonstrates:

object-oriented design in C++
file parsing and structured data loading
route cost evaluation under constraints
implementation of a genetic algorithm
population-based optimization

use of references to reduce unnecessary copying

Author

Created as a university mini-project in C++ / optimization / evolutionary algorithms.
