#pragma once
#include "types.h"
#include <vector>
#include <string>
#include <unordered_map>

// Laadt en beheert dist_matrix.csv
// Rijen/kolommen = locatie-namen (START, END, 02-46, ...)
// Waarden = afstand in willekeurige eenheden (niet symmetrisch door eenrichtingsverkeer)
class DistanceMatrix {
public:
    // Laad vanuit dist_matrix.csv
    // Eerste rij en eerste kolom bevatten locatienamen
    void load(const std::string& filepath);

    // Geef afstand van locatie 'from' naar locatie 'to'
    double get(const LocationID& from, const LocationID& to) const;

    // Bereken de totale afstand van een route:
    // START -> locaties gesorteerd op asis_sequence -> END
    double route_distance(const std::vector<LocationID>& sorted_locations) const;

    int num_locations() const { return static_cast<int>(index_.size()); }

private:
    std::vector<LocationID>                      names_;   // index → naam
    std::unordered_map<LocationID, int>          index_;   // naam  → index
    std::vector<std::vector<double>>             matrix_;  // [from][to]
};
