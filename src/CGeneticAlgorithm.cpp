#include "CGeneticAlgorithm.h"

#include <ctime>

CGeneticAlgorithm::CGeneticAlgorithm(const CEvaluator& evaluator, int pop_size, double cross_prob, double mut_prob, unsigned seed)
    : evaluator_(evaluator), // REFERENCJA => brak kopii
      pop_size_(pop_size),
      cross_prob_(cross_prob),
      mut_prob_(mut_prob),
      rng_(),
      iterations_(0),
      population_(),
      best_ever_() {

    // Uporządkowanie parametrów:
    if (pop_size_ < 2) pop_size_ = 2; // sensowna minimalna populacja
    if (cross_prob_ < 0.0) cross_prob_ = 0.0;
    if (cross_prob_ > 1.0) cross_prob_ = 1.0;
    if (mut_prob_ < 0.0) mut_prob_ = 0.0;
    if (mut_prob_ > 1.0) mut_prob_ = 1.0;

    // Seed RNG:
    if (seed == 0) {
        seed = static_cast<unsigned>(std::time(NULL));
    }
    rng_.seed(seed);
}

void CGeneticAlgorithm::Initialize() {
    iterations_ = 0;

    // clear usuwa elementy, ale pojemność wektora może zostać
    population_.clear();

    // reserve: jedna większa alokacja z góry => mniej realokacji podczas push_back
    population_.reserve(static_cast<size_t>(pop_size_));

    // Długość chromosomu = liczba klientów
    const int len = evaluator_.GetSolutionSize();
    const int gmin = evaluator_.GetLowerBound();
    const int gmax = evaluator_.GetUpperBound();

    // Tworzymy pop_size_ losowych osobników
    for (int i = 0; i < pop_size_; ++i) {
        population_.push_back(CIndividual(len, gmin, gmax, rng_));
    }

    // Liczymy fitness populacji
    EvaluatePopulation();
    best_ever_ = population_[0];
    UpdateBestEver();
}

void CGeneticAlgorithm::EvaluatePopulation() {
    for (size_t i = 0; i < population_.size(); ++i) {
        population_[i].UpdateFitness(evaluator_);
    }
}

void CGeneticAlgorithm::UpdateBestEver() {
    // Przeglądamy populację i jeśli ktoś ma większy fitness -> kopiujemy go do best_ever_
    for (size_t i = 0; i < population_.size(); ++i) {
        if (population_[i].GetFitness() > best_ever_.GetFitness()) {
            best_ever_ = population_[i];
        }
    }
}

const CIndividual& CGeneticAlgorithm::TournamentPick() {
    // Turniej 2-osobnikowy: losujemy dwoch i bierzemy lepszego.
    std::uniform_int_distribution<int> pick(0, static_cast<int>(population_.size() - 1));
    const int a = pick(rng_);
    const int b = pick(rng_);

    if (population_[static_cast<size_t>(a)].GetFitness() >= population_[static_cast<size_t>(b)].GetFitness()) {
        return population_[static_cast<size_t>(a)];
    }
    return population_[static_cast<size_t>(b)];
}

void CGeneticAlgorithm::RunIteration() {
    // 1) ewaluacja (na wszelki wypadek)
    EvaluatePopulation();
    UpdateBestEver();

    const int gmin = evaluator_.GetLowerBound();
    const int gmax = evaluator_.GetUpperBound();

    // 2) tworzenie nowej populacji
    std::vector<CIndividual> next;
    next.reserve(static_cast<size_t>(pop_size_)); // ograniczamy realokacje

    while ((int)next.size() < pop_size_) {
        // p1 i p2 są REFERENCJAMI -> brak kopii
        const CIndividual& p1 = TournamentPick();
        const CIndividual& p2 = TournamentPick();

        // dzieci (lokalne zmienne na stosie)
        CIndividual c1, c2;
        //Krzyżowanie
        CIndividual::CrossoverOnePoint(p1, p2, cross_prob_, rng_, c1, c2);

        // Mutacja gen-po-genu
        c1.Mutate(mut_prob_, gmin, gmax, rng_);
        c2.Mutate(mut_prob_, gmin, gmax, rng_);

        next.push_back(c1);
        if ((int)next.size() < pop_size_) {
            next.push_back(c2);
        }
    }

    population_.swap(next);

    // 3) ewaluacja nowej populacji
    EvaluatePopulation();
    UpdateBestEver();

    iterations_ += 1;
}
