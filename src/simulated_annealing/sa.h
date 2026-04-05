#pragma once
#include "types.h"
#include "distance.h"
#include "evaluator.h"
#include <functional>
#include <string>

// ----- Koelschema's (Deel 1.2) -----

enum class CoolingSchedule {
    GEOMETRIC,   // T(k) = T0 * alpha^k
    LINEAR       // T(k) = T0 - k * delta
};

// ----- Buurbewegingen -----

enum class NeighborType {
    SWAP_ORDERS,    // Wissel twee orders tussen twee routes binnen dezelfde wave
    MOVE_ORDER      // Verplaats één order naar een andere route binnen dezelfde wave
};

// ----- Startoplossing -----

enum class StartSolution {
    RANDOM,
    GREEDY
};

// ----- SA-configuratie -----

struct SAConfig {
    CoolingSchedule cooling      = CoolingSchedule::GEOMETRIC;
    NeighborType    neighbor     = NeighborType::SWAP_ORDERS;
    StartSolution   start        = StartSolution::GREEDY;

    double          T0           = 1000.0;
    double          alpha        = 0.995;   // voor geometrisch schema
    double          delta        = 0.1;     // voor lineair schema
    int             max_iter     = 100'000;
    int             n_max        = 4;
};

// ----- Simulated Annealing solver -----

class SimulatedAnnealing {
public:
    SimulatedAnnealing(const DistanceMatrix& dist, const Evaluator& eval, SAConfig cfg);

    // Voer één volledige SA-run uit; geeft beste gevonden oplossing terug
    Solution solve(const std::vector<Wave>& waves) const;

    // Geeft de convergentiecurve van de laatste run (voor plots)
    const std::vector<double>& convergence_curve() const { return curve_; }

private:
    Solution generate_start(const std::vector<Wave>& waves) const;
    Solution generate_neighbor(const Solution& sol, const std::vector<Wave>& waves) const;
    double   temperature(int iteration) const;

    const DistanceMatrix& dist_;
    const Evaluator&      eval_;
    SAConfig              cfg_;

    mutable std::vector<double> curve_; // beste waarde per iteratie
};
