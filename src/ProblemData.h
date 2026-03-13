#pragma once

#include <string>
#include <vector>


struct Coordinate {
    double x;
    double y;

    Coordinate() : x(0.0), y(0.0) {}
    Coordinate(double ax, double ay) : x(ax), y(ay) {}
};

class ProblemData {
public:
    static const double WRONG_VAL; // -1.0

    ProblemData();

    // podstawowe pola naglowka 
    const std::string& GetName() const { return name_; }

    // liczba węzłów (depot + klienci)
    int GetDimension() const { return dimension_; }  

    // liczba klientów (bez depotu)
    int GetNumCustomers() const { return (dimension_ > 0) ? (dimension_ - 1) : 0; }

    int GetCapacity() const { return capacity_; }

    bool HasDistanceConstraint() const { return has_distance_limit_; }

    double GetDistanceLimit() const { return distance_limit_; }

    const std::string& GetEdgeWeightType() const { return edge_weight_type_; }

    // ID depotu (zwykle 1)
    int GetDepotId() const { return depot_id_; }   

    // dane w sekcjach 
    // const ref -> brak kopii
    const std::vector<Coordinate>& GetCoordinates() const { return coordinates_; }
    const std::vector<int>& GetDemands() const { return demands_; }
    const std::vector<int>& GetPermutation() const { return permutation_; }

    // macierz odleglosci (zawsze symetryczna)
    const std::vector<std::vector<double>>& GetDistances() const { return distances_; }

    // settery (uzywane przez loader)
    void SetName(const std::string& n) { name_ = n; }

    // SetDimension robi też przygotowanie wektorów (alokacje)
    void SetDimension(int dim);

    void SetCapacity(int cap) { capacity_ = cap; }

    // Ustawienie limitu dystansu włącza flagę has_distance_limit_
    void SetDistanceLimit(double lim);

    void SetEdgeWeightType(const std::string& t) { edge_weight_type_ = t; }
    void SetDepotId(int id) { depot_id_ = id; }

    // SetCoordinates/SetDemands/SetPermutation kopiuje wektory do środka
    void SetCoordinates(const std::vector<Coordinate>& c) { coordinates_ = c; }
    void SetDemands(const std::vector<int>& d) { demands_ = d; }
    void SetPermutation(const std::vector<int>& p) { permutation_ = p; }

    // EXPLICIT distances: kopiujemy macierz do środka
    void SetExplicitDistances(const std::vector<std::vector<double>>& m) { distances_ = m; }

    // Buduje macierz distances_ z coordinates_
    void BuildDistancesFromCoordinates();
    // Pobiera dystans po indeksach 0-based
    double DistByIndex(int idxA, int idxB) const;

private:
    std::string name_;
    int dimension_;               // liczba wezlow: depot + klienci
    int capacity_;

    bool has_distance_limit_;
    double distance_limit_;

    std::string edge_weight_type_; // EUC_2D lub EXPLICIT
    int depot_id_;

    // Dane węzłów
    std::vector<Coordinate> coordinates_; // rozmiar = dimension_
    std::vector<int> demands_; // rozmiar = dimension_
    std::vector<int> permutation_; // rozmiar = liczba klientów

    // Macierz dystansów
    std::vector<std::vector<double>> distances_;
};
