#pragma once

#include "CEvaluator.h"
#include "CIndividual.h"
#include <vector>
#include <random>

class CGeneticAlgorithm {
public:
    // evaluator przekazywany jako const ref => BRAK KOPII evaluator'a
    CGeneticAlgorithm(const CEvaluator& evaluator, int pop_size, double cross_prob, double mut_prob, unsigned seed = 0);

    // Tworzy losową populację i liczy jej fitnessy
    void Initialize();

    // Jedna iteracja: selekcja -> krzyżowanie -> mutacja -> ocena
    void RunIteration();

    // Zwraca referencję do best_ever_ => brak kopii przy pobieraniu
    const CIndividual& GetBestEver() const { return best_ever_; }

    // Liczba wykonanych iteracji
    int GetIterations() const { return iterations_; }

private:
    // Referencja do evaluator'a (musi żyć dłużej niż GA)
    const CEvaluator& evaluator_;

    int pop_size_;
    double cross_prob_;
    double mut_prob_;

    std::mt19937 rng_;
    // licznik iteracji wykonanych przez GA
    int iterations_;

    // Populacja i najlepszy osobnik w historii
    std::vector<CIndividual> population_;
    CIndividual best_ever_;

    // Turniej 2-osobnikowy: zwraca referencję do lepszego osobnika (bez kopii)
    const CIndividual& TournamentPick();

    // Liczy fitness każdemu osobnikowi w populacji
    void EvaluatePopulation();

    // Aktualizuje best_ever_ jeśli ktoś w populacji jest lepszy
    void UpdateBestEver();
};
