#include "CEvaluator.h"

#include <cmath>

CEvaluator::CEvaluator() : data_(), num_groups_(0), last_error_("") {}

bool CEvaluator::LoadInstance(const std::string& file_path, int num_groups) {
    last_error_.clear();
    num_groups_ = num_groups;

    // Walidacja parametru wejściowego
    if (num_groups_ <= 0) {
        last_error_ = "Liczba grup (num_groups) musi byc > 0.";
        return false;
    }

    // Loader zwraca true/false i tekst błędu
    ProblemLoader loader;

    ProblemData tmp; // tymczasowy obiekt na wczytaną instancję
    if (!loader.LoadFromFile(file_path, tmp)) {
        last_error_ = loader.GetLastError();
        return false;
    }

    data_ = tmp; //KOPIA CAŁEGO OBIEKTU ProblemData. kopiuje wektory

    // Dodatkowa walidacja instancji pod kątem ograniczeń
    if (!ValidateInstance()) {
        if (last_error_.empty()) last_error_ = "Instancja nie spelnia podstawowych ograniczen.";
        return false;
    }

    return true;
}

bool CEvaluator::ValidateInstance() const {
    // demand <= capacity, a przy ograniczeniu odleglosci: depot->customer->depot <= limit
    const int cap = data_.GetCapacity();
    const int n = data_.GetDimension();
    if (n <= 1 || cap <= 0) return false;

    const int depot_idx = data_.GetDepotId() - 1; // konwersja ID (1..n) -> index (0..n-1)
    if (depot_idx < 0 || depot_idx >= n) return false;

    const std::vector<int>& dem = data_.GetDemands(); // const ref -> brak kopii
    if ((int)dem.size() != n) return false;

    // Klienci są 2..n (1 = depot)
    for (int node_id = 2; node_id <= n; ++node_id) {
        const int idx = node_id - 1;
        if (dem[idx] > cap) {
            // klient ma większy popyt niż capacity -> niemożliwa instancja
            return false;
        }
        if (data_.HasDistanceConstraint()) {
            const double lim = data_.GetDistanceLimit();
            const double d = data_.DistByIndex(depot_idx, idx);
            if (d < 0.0) return false;
            if (2.0 * d > lim) {
                // nie da się nawet pojechać depot->klient->depot
                return false;
            }
        }
    }
    return true;
}

bool CEvaluator::CheckGroupingBounds(const std::vector<int>& grouping) const {
    //grouping musi mieć dokładnie tyle genów, ilu jest klientów
    if ((int)grouping.size() != data_.GetNumCustomers()) return false;

    // Każdy gen musi być w zakresie [0..num_groups-1]
    for (size_t i = 0; i < grouping.size(); ++i) {
        const int g = grouping[i];
        if (g < GetLowerBound() || g > GetUpperBound()) return false;
    }
    return true;
}

void CEvaluator::BuildGroupRoutes(const std::vector<int>& grouping, std::vector< std::vector<int> >& out_routes) const {
    out_routes.clear();
    out_routes.resize(static_cast<size_t>(num_groups_)); // alokacja num_groups wektorów

    const std::vector<int>& perm = data_.GetPermutation(); // const ref = brak kopii

    for (size_t i = 0; i < perm.size(); ++i) {
        const int cust_id = perm[i]; // ID klienta (2..DIMENSION)
        const int cust_index_in_grouping = cust_id - 2; // klient 2 -> index 0
        if (cust_index_in_grouping < 0 || cust_index_in_grouping >= (int)grouping.size()) {
            continue; // gdyby perm była dziwna, pomijamy (bez crasha)
        }
        // Odczyt grupy z genotypu
        const int group = grouping[static_cast<size_t>(cust_index_in_grouping)];

        // push_back kopiuje tylko int (tanie), ale wektor może realokować (dynamiczna pamięć) gdy rośnie
        out_routes[static_cast<size_t>(group)].push_back(cust_id);
    }
}

//Liczenie kosztu dla JEDNEJ grupy (lista klientów)
bool CEvaluator::ComputeRouteCost(const std::vector<int>& customer_ids, double& out_cost) const {
    out_cost = 0.0;
    if (customer_ids.empty()) return true; // pusta grupa => koszt 0

    const int n = data_.GetDimension();
    const int depot_idx = data_.GetDepotId() - 1;
    const int cap = data_.GetCapacity();
    const bool hasLim = data_.HasDistanceConstraint();
    const double lim = hasLim ? data_.GetDistanceLimit() : -1.0;
    const std::vector<int>& dem = data_.GetDemands(); // const ref

    // Stan bieżącego kursu:
    int load = 0; // ile już załadowaliśmy
    double dist = 0.0; // dystans przejechany w bieżącym kursie
    int last_idx = depot_idx; // gdzie aktualnie jesteśmy

    auto closeSubtour = [&]() -> bool {
        const double back = data_.DistByIndex(last_idx, depot_idx);
        if (back < 0.0) return false; // błąd w macierzy odległości
        dist += back; // wracamy do depotu
        out_cost += dist; // dopisujemy koszt kursu do sumy

        // reset
        load = 0;
        dist = 0.0;
        last_idx = depot_idx;
        return true;
    };

    // Iterujemy po klientach tej grupy:
    for (size_t i = 0; i < customer_ids.size(); ++i) {
        const int cust_id = customer_ids[i];
        if (cust_id < 2 || cust_id > n) return false; // ID klienta poza zakresem
        const int cust_idx = cust_id - 1; // ID -> index

        const int d = dem[static_cast<size_t>(cust_idx)]; // demand klienta

        // OGRANICZENIE POJEMNOŚCI
        if (load + d > cap) {
            if (!closeSubtour()) return false;
        }

        // krok: dystans z aktualnej pozycji do klienta
        double step = data_.DistByIndex(last_idx, cust_idx);
        if (step < 0.0) return false;

        // Jesli isthieje ograniczenie dystansu
        if (hasLim) {
            const double back = data_.DistByIndex(cust_idx, depot_idx);
            if (back < 0.0) return false;

            // Sprawdzamy czy po dojechaniu do klienta i wróceniu do depotu nie przekroczymy limitu
            if (dist + step + back > lim) {
                // Jeśli przekroczymy, zamykamy kurs i zaczynamy nowy od depotu
                if (!closeSubtour()) return false;

                // Teraz step liczymy z depotu do klienta
                step = data_.DistByIndex(depot_idx, cust_idx);
                if (step < 0.0) return false;
            }
        }

        // Aktualizacja stanu kursu:
        dist += step;  // dodajemy dystans przejazdu do klienta
        load += d;  // dodajemy demand do ładunku
        last_idx = cust_idx;  // jesteśmy teraz u klienta
    }

    // domkniecie ostatniej podtrasy
    return closeSubtour();
}

bool CEvaluator::EvaluateCost(const std::vector<int>& grouping, double& out_cost) const {
    out_cost = ProblemData::WRONG_VAL;

    // Bez poprawnej instancji nie liczymy nic.
    if (!ValidateInstance()) return false;

    // Bez poprawnego genotypu też nie
    if (!CheckGroupingBounds(grouping)) return false;

    // Budujemy listy klientów w każdej grupie wg PERMUTATION
    std::vector< std::vector<int> > routes;
    BuildGroupRoutes(grouping, routes);

    // Sumujemy koszty wszystkich grup
    double total = 0.0;
    for (size_t g = 0; g < routes.size(); ++g) {
        double routeCost = 0.0;
        if (!ComputeRouteCost(routes[g], routeCost)) return false;
        total += routeCost;
    }

    out_cost = total;
    return true;
}

double CEvaluator::EvaluateFitness(const std::vector<int>& grouping) const {
    double cost = 0.0;
    if (!EvaluateCost(grouping, cost)) return 0.0;

    // przeskalowanie: im mniejszy koszt, tym wiekszy fitness
    return 1.0 / (1.0 + cost);
}
