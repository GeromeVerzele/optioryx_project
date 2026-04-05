#pragma once
#include "types.h"
#include "distance.h"
#include "evaluator.h"
#include <string>

// Deel 2 – Seed-and-Selection heuristiek
// Gebaseerd op: Ho, Su & Shi – "Order-batching methods for an order-picking
// warehouse with two cross aisles"
//
// Aanpassing aan onze setting:
//   - Capaciteit = n_max ORDERS (niet items)
//   - Pickvolgorde is vast via snake-pattern

// ----- Seed rules -----
// Bepaalt welke order als startpunt (zaad) van een nieuwe batch wordt gekozen.

enum class SeedRule {
    FIRST_ORDER,    // Eerste ongebatchte order (eenvoudigste baseline)
    FARTHEST,       // Order met het item verst van het depot → lange routes eerst bundelen
    // TODO: eigen seed rule
    CUSTOM          // Eigen ontwerp (Deel 2.4)
};

// ----- Selection rules -----
// Bepaalt welke volgende order aan de lopende batch wordt toegevoegd.

enum class SelectionRule {
    CLOSEST_OVERLAP,  // Order met meeste overlap in snake-positie met huidige batch
    MINIMUM_ADDITION, // Order die de minste extra afstand toevoegt aan de route
    // TODO: eigen selection rule
    CUSTOM            // Eigen ontwerp (Deel 2.4)
};

// ----- Configuratie -----

struct SeedSelectionConfig {
    SeedRule      seed      = SeedRule::FARTHEST;
    SelectionRule selection = SelectionRule::CLOSEST_OVERLAP;
    int           n_max     = 4;
};

// ----- Solver -----

class SeedSelectionBatcher {
public:
    SeedSelectionBatcher(const DistanceMatrix& dist, const Evaluator& eval,
                         SeedSelectionConfig cfg);

    Solution solve(const std::vector<Wave>& waves) const;

private:
    std::vector<Route> batch_wave(const Wave& wave) const;

    // Kies het zaad uit de resterende orders
    OrderID choose_seed(const std::vector<Order>& remaining,
                        const Wave& wave) const;

    // Kies de volgende order om aan de batch toe te voegen
    OrderID choose_next(const Route& current_batch,
                        const std::vector<Order>& remaining) const;

    const DistanceMatrix&  dist_;
    const Evaluator&       eval_;
    SeedSelectionConfig    cfg_;
};
