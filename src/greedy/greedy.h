#pragma once
#include "types.h"
#include "distance.h"
#include "evaluator.h"

// Deel 1.1 – Greedy heuristiek
//
// Greedy keuze: bij het opbouwen van een batch voeg telkens de ongebatchte order
// toe die de minste marginale wandelafstand toevoegt aan de huidige route.
// = cheapest insertion in de pick-sequentie.
//
// Stappen per wave:
//   1. Kies de eerste order als zaad van een nieuwe batch.
//   2. Voeg herhaaldelijk de order toe met de laagste marginale kost
//      (= route_distance(batch ∪ {candidate}) - route_distance(batch))
//      totdat de batch vol is (n_max) of er geen orders meer zijn.
//   3. Start een nieuwe batch.

class GreedyBatcher {
public:
    GreedyBatcher(const DistanceMatrix& dist, const Evaluator& eval, int n_max);

    Solution solve(const std::vector<Wave>& waves) const;

private:
    std::vector<Route> batch_wave(const Wave& wave, int& list_id_counter) const;

    // Extra wandelafstand als 'candidate' aan de huidige route wordt toegevoegd
    double marginal_cost(const Route& route, const Order& candidate,
                         const std::vector<Wave>& waves) const;

    const DistanceMatrix& dist_;
    const Evaluator&      eval_;
    int                   n_max_;
};
