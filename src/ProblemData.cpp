#include "ProblemData.h"

#include <cmath>

const double ProblemData::WRONG_VAL = -1.0;

ProblemData::ProblemData()
    : name_(""),
      dimension_(0),
      capacity_(0),
      has_distance_limit_(false),
      distance_limit_(0.0),
      edge_weight_type_(""),
      depot_id_(1) {}

void ProblemData::SetDimension(int dim) {
    // Ustawiamy dimension i przygotowujemy struktury o odpowiednim rozmiarze
    dimension_ = dim;
    if (dimension_ < 0) dimension_ = 0;

    coordinates_.assign(static_cast<size_t>(dimension_), Coordinate());
    demands_.assign(static_cast<size_t>(dimension_), 0);

    // Czyścimy macierz dystansów — zbudujemy ją później
    distances_.clear();
}

void ProblemData::SetDistanceLimit(double lim) {
    distance_limit_ = lim;
    has_distance_limit_ = true; // ustawienie flagi, że constraint istnieje
}

void ProblemData::BuildDistancesFromCoordinates() {
    // Buduje distances_ jako pełną macierz n x n

    if (dimension_ <= 0) return;
    if (coordinates_.size() != static_cast<size_t>(dimension_)) return;

    distances_.assign(static_cast<size_t>(dimension_), std::vector<double>(static_cast<size_t>(dimension_), 0.0));

    for (int i = 0; i < dimension_; ++i) {
        for (int j = 0; j < dimension_; ++j) {
            if (i == j) {
                distances_[i][j] = 0.0;
            } else {
                const double dx = coordinates_[i].x - coordinates_[j].x;
                const double dy = coordinates_[i].y - coordinates_[j].y;
                distances_[i][j] = std::sqrt(dx * dx + dy * dy);
            }
        }
    }
}

double ProblemData::DistByIndex(int idxA, int idxB) const {
    // Bezpieczne pobieranie dystansu: sprawdzamy zakres i czy macierz istnieje
    if (idxA < 0 || idxB < 0) return WRONG_VAL;
    if (idxA >= dimension_ || idxB >= dimension_) return WRONG_VAL;
    if (distances_.empty()) return WRONG_VAL;
    return distances_[idxA][idxB];
}
