#include "ProblemLoader.h"

#include <fstream>
#include <sstream>

ProblemLoader::ProblemLoader() : last_error_("") {}

static bool StartsWith(const std::string& s, const std::string& prefix) {
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

std::string ProblemLoader::Trim(const std::string& s) {
    // usuwa spacje, taby, CR, LF z obu stron
    const char* ws = " \t\r\n";
    const size_t a = s.find_first_not_of(ws);
    if (a == std::string::npos) return "";
    const size_t b = s.find_last_not_of(ws);
    return s.substr(a, b - a + 1);
}

bool ProblemLoader::LoadFromFile(const std::string& file_path, ProblemData& out_data) {
    last_error_.clear();

    out_data = ProblemData(); //kopia/przypisanie pustego obiektu, żeby zawsze startować z czystym stanem

    std::ifstream f(file_path.c_str());
    if (!f.is_open()) {
        last_error_ = "Nie mozna otworzyc pliku: " + file_path;
        return false;
    }

    std::string line;

    // Czytamy plik linia po linii
    while (std::getline(f, line)) {
        line = Trim(line);
        if (line.empty()) continue;

        // EOF kończy instancję
        if (line.find("EOF") != std::string::npos) {
            break;
        }

        // Sekcje, gdy trafimy na nazwę sekcji, wchodzimy w odpowiedni parser
        if (line.find("EDGE_WEIGHT_SECTION") != std::string::npos) {
            if (!ParseEdgeWeightSection(f, out_data)) return false;
            continue;
        }
        if (line.find("NODE_COORD_SECTION") != std::string::npos) {
            if (!ParseNodeCoordSection(f, out_data)) return false;
            continue;
        }
        if (line.find("DEMAND_SECTION") != std::string::npos) {
            if (!ParseDemandSection(f, out_data)) return false;
            continue;
        }
        if (line.find("DEPOT_SECTION") != std::string::npos) {
            if (!ParseDepotSection(f, out_data)) return false;
            continue;
        }

        // Naglowki (KEY: VALUE)
        if (!ParseHeaderLine(line, out_data)) {
            // nieznana linia - ignorujemy
        }
    }

    // Budowa macierzy odleglosci (EUC_2D)
    if (out_data.GetEdgeWeightType() == "EUC_2D") {
        out_data.BuildDistancesFromCoordinates();
    }

    // Minimalna walidacja
    if (out_data.GetDimension() <= 1) {
        last_error_ = "Brak poprawnego pola DIMENSION.";
        return false;
    }
    if (out_data.GetCapacity() <= 0) {
        last_error_ = "Brak poprawnego pola CAPACITY.";
        return false;
    }
    if (out_data.GetPermutation().empty()) {
        last_error_ = "Brak pola PERMUTATION.";
        return false;
    }
    if (out_data.GetDistances().empty()) {
        last_error_ = "Nie zbudowano macierzy odleglosci (brak wspolrzednych lub EDGE_WEIGHT_SECTION).";
        return false;
    }

    return true;
}

bool ProblemLoader::ParseHeaderLine(const std::string& line, ProblemData& data) {
    // Szukamy dwukropka w formacie: KEY : VALUE
    const size_t colon = line.find(':');
    if (colon == std::string::npos) return false;

    const std::string key = Trim(line.substr(0, colon));
    const std::string val = Trim(line.substr(colon + 1));

    // Każdy setter kopiuje dane do ProblemData
    if (key == "NAME") {
        data.SetName(val);
        return true;
    }
    if (key == "DIMENSION") {
        std::istringstream iss(val);
        int dim = 0;
        iss >> dim;
        data.SetDimension(dim); // tu następują alokacje w ProblemData
        return true;
    }
    if (key == "CAPACITY") {
        std::istringstream iss(val);
        int cap = 0;
        iss >> cap;
        data.SetCapacity(cap);
        return true;
    }
    if (key == "DISTANCE") {
        std::istringstream iss(val);
        double d = 0.0;
        iss >> d;
        data.SetDistanceLimit(d);
        return true;
    }
    if (key == "EDGE_WEIGHT_TYPE") {
        data.SetEdgeWeightType(val);
        return true;
    }
    if (key == "PERMUTATION") {
        std::vector<int> perm;
        std::istringstream iss(val);
        int v = 0;
        while (iss >> v) perm.push_back(v);

        // SetPermutation kopiuje wektor do środka ProblemData
        data.SetPermutation(perm);
        return true;
    }

    return false; // inne klucze ignorujemy
}

bool ProblemLoader::ParseEdgeWeightSection(std::istream& in, ProblemData& data) {
    const int n = data.GetDimension();
    if (n <= 0) {
        last_error_ = "EDGE_WEIGHT_SECTION przed DIMENSION.";
        return false;
    }

    // Tworzymy macierz n x n w pamięci
    std::vector<std::vector<double>> m(static_cast<size_t>(n), std::vector<double>(static_cast<size_t>(n), 0.0));

    // w pliku jest macierz trojkatna (dolna) bez przekatnej
    for (int i = 1; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            double w = 0.0;
            if (!(in >> w)) {
                last_error_ = "Nieudany odczyt EDGE_WEIGHT_SECTION.";
                return false;
            }
            // uzupełniamy symetrycznie
            m[i][j] = w;
            m[j][i] = w;
        }
    }

    // SetExplicitDistances kopiuje macierz do ProblemData
    data.SetExplicitDistances(m);
    return true;
}

bool ProblemLoader::ParseNodeCoordSection(std::istream& in, ProblemData& data) {
    const int n = data.GetDimension();
    if (n <= 0) {
        last_error_ = "NODE_COORD_SECTION przed DIMENSION.";
        return false;
    }

    // coords ma rozmiar n
    std::vector<Coordinate> coords(static_cast<size_t>(n));

    // w sekcji powinno być n wierszy: node_id x y
    for (int i = 0; i < n; ++i) {
        int node_id = 0;
        double x = 0.0, y = 0.0;
        if (!(in >> node_id >> x >> y)) {
            last_error_ = "Nieudany odczyt NODE_COORD_SECTION.";
            return false;
        }
        if (node_id < 1 || node_id > n) {
            last_error_ = "NODE_COORD_SECTION: niepoprawny node_id.";
            return false;
        }
        // zapisujemy do indeksu 0-based
        coords[static_cast<size_t>(node_id - 1)] = Coordinate(x, y);
    }

    // kopia coords do ProblemData
    data.SetCoordinates(coords);
    return true;
}

bool ProblemLoader::ParseDemandSection(std::istream& in, ProblemData& data) {
    const int n = data.GetDimension();
    if (n <= 0) {
        last_error_ = "DEMAND_SECTION przed DIMENSION.";
        return false;
    }

    std::vector<int> demands(static_cast<size_t>(n), 0);

    // sekcja powinna mieć n wierszy: node_id demand
    for (int i = 0; i < n; ++i) {
        int node_id = 0;
        int dem = 0;
        if (!(in >> node_id >> dem)) {
            last_error_ = "Nieudany odczyt DEMAND_SECTION.";
            return false;
        }
        if (node_id < 1 || node_id > n) {
            last_error_ = "DEMAND_SECTION: niepoprawny node_id.";
            return false;
        }
        demands[static_cast<size_t>(node_id - 1)] = dem;
    }

    // kopia wektora do ProblemData
    data.SetDemands(demands);
    return true;
}

bool ProblemLoader::ParseDepotSection(std::istream& in, ProblemData& data) {
    int depot = 0;
    if (!(in >> depot)) {
        last_error_ = "Nieudany odczyt DEPOT_SECTION.";
        return false;
    }
    data.SetDepotId(depot);

    // standardowo nastepuje -1 jako terminator
    int term = 0;
    if (!(in >> term)) {
        last_error_ = "DEPOT_SECTION: brak terminatora.";
        return false;
    }
    return true;
}
