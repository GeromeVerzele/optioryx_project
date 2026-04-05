#include "distance.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

// Verwijder \r, \n en spaties aan begin/einde
static std::string trim_cell(std::string s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' '))
        s.pop_back();
    size_t start = s.find_first_not_of(" \t");
    if (start == std::string::npos) return "";
    return s.substr(start);
}

void DistanceMatrix::load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("Kan dist_matrix.csv niet openen: " + filepath);

    std::string line;

    // Eerste rij = header: eerste cel leeg/index, daarna locatienamen
    std::getline(file, line);
    {
        std::stringstream ss(line);
        std::string cell;
        std::getline(ss, cell, ','); // eerste cel overslaan (leeg of index-naam)
        while (std::getline(ss, cell, ',')) {
            cell = trim_cell(cell);
            if (cell.empty()) continue;
            index_[cell] = static_cast<int>(names_.size());
            names_.push_back(cell);
        }
    }

    std::cout << "      dist_matrix: " << names_.size() << " locaties geladen.\n";

    int n = static_cast<int>(names_.size());
    matrix_.assign(n, std::vector<double>(n, 0.0));

    int row_num = 0;
    while (std::getline(file, line)) {
        line = trim_cell(line);
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string cell;

        // Eerste cel = rijnaam (locatie)
        std::getline(ss, cell, ',');
        cell = trim_cell(cell);
        if (cell.empty()) continue;

        auto it = index_.find(cell);
        if (it == index_.end()) {
            std::cerr << "[WAARSCHUWING] Onbekende locatie in dist_matrix rij: '" << cell << "'\n";
            continue;
        }
        int row = it->second;

        int col = 0;
        while (std::getline(ss, cell, ',') && col < n) {
            cell = trim_cell(cell);
            if (!cell.empty()) {
                try {
                    matrix_[row][col] = std::stod(cell);
                } catch (...) {
                    // lege cel of tekst â†’ afstand 0
                }
            }
            ++col;
        }
        ++row_num;
    }
    std::cout << "      dist_matrix: " << row_num << " rijen verwerkt.\n";
}

double DistanceMatrix::get(const LocationID& from, const LocationID& to) const {
    return matrix_[index_.at(from)][index_.at(to)];
}

double DistanceMatrix::route_distance(const std::vector<LocationID>& locs) const {
    // START â†’ locs (gesorteerd op asis_seq, al aangeleverd) â†’ END
    double dist = 0.0;
    LocationID prev = "START";
    for (const auto& loc : locs) {
        dist += get(prev, loc);
        prev = loc;
    }
    dist += get(prev, "END");
    return dist;
}