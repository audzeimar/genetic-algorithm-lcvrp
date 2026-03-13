#pragma once

#include "CEvaluator.h"
#include <vector>
#include <random>

//  CIndividual: Reprezentuje JEDNO rozwiązanie
class CIndividual {
public:
    CIndividual(); // pusty osobnik (genotype pusty)
    CIndividual(int genome_len, int gene_min, int gene_max, std::mt19937& rng);

    // Zwracamy const ref => brak kopiowania genotypu przy odczycie
    const std::vector<int>& GetGenotype() const { return genotype_; }

    // przystosowanie i koszt
    void UpdateFitness(const CEvaluator& evaluator);

    double GetFitness() const { return fitness_; }
    double GetCost() const { return cost_; }

    // mutacja gen-po-genu
    void Mutate(double mut_prob, int gene_min, int gene_max, std::mt19937& rng);

    // krzyzowanie 1-punktowe (zwraca 2 dzieci)
    static void CrossoverOnePoint(
        const CIndividual& parentA,
        const CIndividual& parentB,
        double cross_prob,
        std::mt19937& rng,
        CIndividual& out_child1,
        CIndividual& out_child2
    );

private:
    std::vector<int> genotype_;

    double fitness_;
    double cost_;

    bool evaluated_; // informacja czy cost_/fitness_ odpowiadają genotype_
};
