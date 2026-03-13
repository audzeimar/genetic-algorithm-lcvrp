#include "CIndividual.h"

// pusty osobnik: genotype_ jest pusty
CIndividual::CIndividual()
    : genotype_(), fitness_(0.0), cost_(ProblemData::WRONG_VAL), evaluated_(false) {} 

CIndividual::CIndividual(int genome_len, int gene_min, int gene_max, std::mt19937& rng)
    : genotype_(), fitness_(0.0), cost_(ProblemData::WRONG_VAL), evaluated_(false) {
    if (genome_len < 0) genome_len = 0;

    //resize: alokuje pamięć na genome_len el., wypełnia gene_min jako wartość początkową
    genotype_.resize(static_cast<size_t>(genome_len), gene_min);

    // Rozkład losowy dla genów
    std::uniform_int_distribution<int> dist(gene_min, gene_max);

    // Losujemy każdy gen niezależnie
    for (int i = 0; i < genome_len; ++i) {
        genotype_[static_cast<size_t>(i)] = dist(rng);
    }
}

void CIndividual::UpdateFitness(const CEvaluator& evaluator) {
    double c = 0.0;
    if (evaluator.EvaluateCost(genotype_, c)) {
        cost_ = c;
        fitness_ = 1.0 / (1.0 + cost_);
    } else {
        // kara za niepoprawne rozwiązanie
        cost_ = ProblemData::WRONG_VAL;
        fitness_ = 0.0;
    }
    evaluated_ = true;
}

void CIndividual::Mutate(double mut_prob, int gene_min, int gene_max, std::mt19937& rng) {
    if (mut_prob <= 0.0) return;
    if (genotype_.empty()) return;

    // u01: los w [0,1]
    std::uniform_real_distribution<double> u01(0.0, 1.0);
    // dist: nowa wartość genu z [gene_min..gene_max]
    std::uniform_int_distribution<int> dist(gene_min, gene_max);

    // Mutacja gen-po-genu
    for (size_t i = 0; i < genotype_.size(); ++i) {
        if (u01(rng) < mut_prob) {
            genotype_[i] = dist(rng);
        }
    }

    // genotyp się zmienił => zapisany fitness/cost nie pasuje
    evaluated_ = false;
}

void CIndividual::CrossoverOnePoint(
    const CIndividual& parentA,
    const CIndividual& parentB,
    double cross_prob,
    std::mt19937& rng,
    CIndividual& out_child1,
    CIndividual& out_child2
) {
    //kopie rodziców (kopiuje też genotype_)
    out_child1 = parentA;
    out_child2 = parentB;

    // Rozmiar chromosomu
    const size_t n = parentA.genotype_.size();

    // Jeśli nie ma genów albo rodzice mają różną długość -> kończymy
    if (n == 0 || parentB.genotype_.size() != n) {
        out_child1.evaluated_ = false;
        out_child2.evaluated_ = false;
        return;
    }

    // Jeśli cross_prob == 0 lub los >= cross_prob -> brak krzyżowania (dzieci = kopie rodziców)
    std::uniform_real_distribution<double> u01(0.0, 1.0);
    if (cross_prob <= 0.0 || u01(rng) >= cross_prob) {
        out_child1.evaluated_ = false;
        out_child2.evaluated_ = false;
        return;
    }

    // punkt w zakresie [1, n-1]
    std::uniform_int_distribution<int> cutDist(1, static_cast<int>(n - 1));
    const int cut = cutDist(rng);

    for (int i = cut; i < (int)n; ++i) {
        out_child1.genotype_[static_cast<size_t>(i)] = parentB.genotype_[static_cast<size_t>(i)];
        out_child2.genotype_[static_cast<size_t>(i)] = parentA.genotype_[static_cast<size_t>(i)];
    }

    // Po krzyżowaniu fitness już nieaktualny
    out_child1.evaluated_ = false;
    out_child2.evaluated_ = false;
}
