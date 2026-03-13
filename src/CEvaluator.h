#pragma once

#include "ProblemData.h"
#include "ProblemLoader.h"
#include <string>
#include <vector>


class CEvaluator {
public:
    CEvaluator();

    //LoadInstance: wczyt plik.lcvrp poprzez ProblemLoader, ustawia num_groups_, sprawdza minimalne warunki(ValidateInstance).
    bool LoadInstance(const std::string& file_path, int num_groups);

    // Koszt rozwiązania (suma długości tras). Zwraca false gdy rozwiązanie niepoprawne.
    bool EvaluateCost(const std::vector<int>& grouping, double& out_cost) const;

    // Fitness w [0,1]. Dla niepoprawnych rozwiązań fitness=0.
    double EvaluateFitness(const std::vector<int>& grouping) const;

    // Rozmiar rozwiązania = liczba klientów = DIMENSION-1
    int GetSolutionSize() const { return data_.GetNumCustomers(); }

    // Dla genów: zakres wartości grup
    int GetLowerBound() const { return 0; }
    int GetUpperBound() const { return (num_groups_ > 0) ? (num_groups_ - 1) : 0; }
    int GetNumGroups() const { return num_groups_; }

    const std::string& GetLastError() const { return last_error_; } // const ref = brak kopii
    const ProblemData& GetProblemData() const { return data_; } // const ref = brak kopii

private:
    ProblemData data_;
    int num_groups_; //ile grup ma mieć genotyp
    std::string last_error_;

    // Sprawdza minimalnie sens instancji
    bool ValidateInstance() const;

    // Sprawdza czy grouping ma dobry rozmiar i wartości w zakresie [0..k-1]
    bool CheckGroupingBounds(const std::vector<int>& grouping) const;

    //Buduje listy klientów dla każdej grupy
    void BuildGroupRoutes(const std::vector<int>& grouping, std::vector< std::vector<int> >& out_routes) const;
    
    //Liczy koszt przejazdu dla JEDNEJ grupy (jednej listy klientów)
    bool ComputeRouteCost(const std::vector<int>& customer_ids, double& out_cost) const;
};
