#include "CEvaluator.h"
#include "CGeneticAlgorithm.h"

#include <iostream>
#include <sstream>

//bierze tekst i próbuje go sparsować do liczby
//nie rzuca wyjątków, tylko zwraca false jak się nie uda

static bool ParseInt(const char* s, int& out) {
    if (!s) return false; // jeśli wskaźnik null -> nie ma co parsować
    std::istringstream iss(s); 
    iss >> out;
    return !iss.fail(); // jeśli fail() = true -> błąd parsowania
}

static bool ParseDouble(const char* s, double& out) {
    if (!s) return false;
    std::istringstream iss(s);
    iss >> out;
    return !iss.fail();
}

int main(int argc, char** argv) {
    // Interfejs tekstowy jest tylko w main (zgodnie z trescia)

    std::string instance_path = "data/A-n32-k5.lcvrp";
    int num_groups = 5;

    // Parametry GA
    int pop_size = 100; // rozmiar populacji
    double cross_prob = 0.6; // prawdopodobieństwo krzyżowania
    double mut_prob = 0.1; // prawdopodobieństwo mutacji genu
    int iterations = 200; // liczba iteracji
    unsigned seed = 0;  // seed RNG (0 => seed z time())

    if (argc >= 2) instance_path = argv[1];
    if (argc >= 3) ParseInt(argv[2], num_groups);
    if (argc >= 4) ParseInt(argv[3], pop_size);
    if (argc >= 5) ParseDouble(argv[4], cross_prob);
    if (argc >= 6) ParseDouble(argv[5], mut_prob);
    if (argc >= 7) ParseInt(argv[6], iterations);
    if (argc >= 8) {
        int tmp = 0;
        if (ParseInt(argv[7], tmp) && tmp >= 0) seed = static_cast<unsigned>(tmp);
    }

    CEvaluator eval;

    //Wczytanie instancji + fallback
    if (!eval.LoadInstance(instance_path, num_groups)) {
        if (argc < 2 && instance_path == "data/A-n32-k5.lcvrp") {
            const std::string alt = "../data/A-n32-k5.lcvrp";
            if (eval.LoadInstance(alt, num_groups)) {
                instance_path = alt;
            }
        }
    }

    //jeśli jest błąd i nie ma sensownej instancji (DIMENSION==0), to kończymy program
    if (!eval.GetLastError().empty() && eval.GetProblemData().GetDimension() == 0) {
        std::cerr << "BLAD wczytywania instancji: " << eval.GetLastError() << "\n";
        std::cerr << "Uzycie: tep_mini_ga_lcvrp <sciezka_do_.lcvrp> <num_grup> [pop] [cross] [mut] [iter] [seed]\n";
        return 1;
    }

    //GA dostaje evaluator PRZEZ REFERENCJĘ(const CEvaluator&) - NIE ma kopiowania evaluator'a.
    CGeneticAlgorithm ga(eval, pop_size, cross_prob, mut_prob, seed);

    // Inicjalizacja: tworzy losową populację i liczy fitnessy.
    ga.Initialize();

    // Pętla iteracji GA
    for (int i = 0; i < iterations; ++i) {
        ga.RunIteration();
    }

    //best jest referencją do obiektu trzymanego w ga, więc: brak kopiowania osobnika
    const CIndividual& best = ga.GetBestEver();

    std::cout << "Instancja: " << eval.GetProblemData().GetName() << "\n";
    std::cout << "Wymiary (wezly): " << eval.GetProblemData().GetDimension() << "  (klienci: " << eval.GetSolutionSize() << ")\n";
    std::cout << "Grupy: " << eval.GetNumGroups() << "\n";
    std::cout << "Iteracje GA: " << ga.GetIterations() << "\n\n";

    std::cout << "Najlepszy osobnik (fitness = 1/(1+cost)):\n";
    std::cout << "  fitness = " << best.GetFitness() << "\n";
    std::cout << "  cost    = " << best.GetCost() << "\n";

    std::cout << "  genotyp (pierwsze 50 genow): ";
    // GetGenotype() zwraca const ref -> brak kopiowania genotypu
    const std::vector<int>& g = best.GetGenotype();
    for (size_t i = 0; i < g.size() && i < 50; ++i) {
        std::cout << g[i];
        if (i + 1 < g.size() && i + 1 < 50) std::cout << ' ';
    }
    if (g.size() > 50) std::cout << " ...";
    std::cout << "\n";

    return 0;
}
