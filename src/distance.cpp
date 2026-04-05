#include "distance.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

void DistanceMatrix::load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("Kan dist_matrix.csv niet openen: " + filepath);

    std::string line;

    // Eerste rij = header: eerste cel leeg, daarna locatienamen
    std::getline(file, line);
    {
        std::stringstream ss(line);
        std::string cell;
        std::getline(ss, cell, ','); // lege eerste cel overslaan
        while (std::getline(ss, cell, ',')) {
            if (!cell.empty() && cell.back() == '\r') cell.pop_back();
            index_[cell] = static_cast<int>(names_.size());
            names_.push_back(cell);
        }
    }

    int n = static_cast<int>(names_.size());
    matrix_.assign(n, std::vector<double>(n, 0.0));

    // Datarijens: eerste cel = rijnaam, daarna afstanden
    while (std::getline(file, line)) {
        if (line.empty() || line == "\r") continue;
        std::stringstream ss(line);
        std::string cell;

        std::getline(ss, cell, ',');
        if (!cell.empty() && cell.back() == '\r') cell.pop_back();
        int row = index_.at(cell);

        int col = 0;
        while (std::getline(ss, cell, ',')) {
            if (!cell.empty() && cell.back() == '\r') cell.pop_back();
            matrix_[row][col++] = std::stod(cell);
        }
    }
}

double DistanceMatrix::get(const LocationID& from, const LocationID& to) const {
    return matrix_[index_.at(from)][index_.at(to)];
}

double DistanceMatrix::route_distance(const std::vector<LocationID>& locs) const {
    // START → locs (gesorteerd op asis_seq, al aangeleverd) → END
    double dist = 0.0;
    LocationID prev = "START";
    for (const auto& loc : locs) {
        dist += get(prev, loc);
        prev = loc;
    }
    dist += get(prev, "END");
    return dist;
}
