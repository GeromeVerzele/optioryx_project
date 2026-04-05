#pragma once
#include "types.h"
#include "distance.h"

// Berekent de score zoals Kaggle die meet:
//   Per lijst: verzamel unieke locaties van alle orders,
//              sorteer op ASIS_SEQUENCE,
//              loop START → locaties → END,
//              tel afstanden op uit dist_matrix.csv.
//   Eindscore = som over alle lijsten.
class Evaluator {
public:
    explicit Evaluator(const DistanceMatrix& dist);

    // Totale score van een volledige oplossing (lager = beter)
    double total_distance(const Solution& sol,
                          const std::vector<Wave>& waves) const;

    // Score van één enkele route
    double route_distance(const Route& route,
                          const std::vector<Wave>& waves) const;

    // Valideert alle Kaggle-constraints (n_max, één wave per lijst, etc.)
    bool is_valid(const Solution& sol,
                  const std::vector<Wave>& waves) const;

private:
    // Verzamel alle unieke locaties van een route, gesorteerd op asis_sequence
    std::vector<LocationID> collect_locations(
        const Route& route,
        const std::vector<Wave>& waves) const;

    const DistanceMatrix& dist_;
};
