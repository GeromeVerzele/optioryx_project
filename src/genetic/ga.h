#pragma once
#include "types.h"
#include "distance.h"
#include "evaluator.h"
#include <vector>

// Deel 1.3 – Genetisch Algoritme
//
// Representatie: één individu = een volledige Solution (vector van waves, elk met routes).
// Fitness = totale wandelafstand (minimaliseren).

struct GAConfig {
    int    population_size  = 100;
    int    generations      = 500;
    double crossover_rate   = 0.8;
    double mutation_rate    = 0.1;
    int    tournament_size  = 5;
    int    elitism_count    = 2;   // beste individuen bewaren (elitisme)
    int    n_max            = 4;
};

class GeneticAlgorithm {
public:
    GeneticAlgorithm(const DistanceMatrix& dist, const Evaluator& eval, GAConfig cfg);

    // Voer het GA uit; geeft beste gevonden oplossing terug
    Solution solve(const std::vector<Wave>& waves) const;

    // Convergentiecurve: beste fitness per generatie
    const std::vector<double>& convergence_curve() const { return curve_; }

private:
    // Initialiseer populatie (mix van greedy + random)
    std::vector<Solution> init_population(const std::vector<Wave>& waves) const;

    // Toernooikeuze
    const Solution& tournament_select(const std::vector<Solution>& pop,
                                      const std::vector<double>&   fitness) const;

    // Crossover: combineert toewijzing van orders uit twee ouders
    Solution crossover(const Solution& p1, const Solution& p2,
                       const std::vector<Wave>& waves) const;

    // Mutatie: wissel of verplaats willekeurige orders
    Solution mutate(const Solution& sol, const std::vector<Wave>& waves) const;

    // Fitness = totale wandelafstand (lager = beter)
    double fitness(const Solution& sol) const;

    const DistanceMatrix& dist_;
    const Evaluator&      eval_;
    GAConfig              cfg_;

    mutable std::vector<double> curve_;
};
