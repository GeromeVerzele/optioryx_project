#include "greedy/greedy.h"
#include <limits>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

GreedyBatcher::GreedyBatcher(const DistanceMatrix& dist,
                             const Evaluator& eval, int n_max)
    : dist_(dist), eval_(eval), n_max_(n_max) {}

Solution GreedyBatcher::solve(const std::vector<Wave>& waves) const {
    Solution sol;
    int list_id = 1;
    int wi = 0;
    for (const Wave& w : waves) {
        std::cout << "      Wave " << ++wi << "/" << waves.size()
                  << " (" << w.orders.size() << " orders)...\n";
        sol.push_back(batch_wave(w, list_id));
    }
    return sol;
}

// Bereken de wandelafstand voor een set locaties (al gesorteerd op ASIS-volgorde)
// START → locs → END
static double compute_distance(const DistanceMatrix& dist,
                                const std::vector<LocationID>& locs) {
    return dist.route_distance(locs);
}

// Verzamel unieke locaties van een batch, gegeven een pre-built order lookup
static std::vector<LocationID> collect_locs(
    const std::vector<OrderID>& order_ids,
    const std::unordered_map<OrderID, const Order*>& lookup)
{
    std::unordered_set<LocationID> seen;
    std::vector<LocationID> result;
    for (const OrderID& oid : order_ids) {
        auto it = lookup.find(oid);
        if (it == lookup.end()) continue;
        for (const LocationID& loc : it->second->locations)
            if (seen.insert(loc).second)
                result.push_back(loc);
    }
    return result;
}

std::vector<Route> GreedyBatcher::batch_wave(const Wave& wave,
                                              int& list_id_counter) const {
    // Pre-build order lookup EENMALIG voor deze wave
    std::unordered_map<OrderID, const Order*> lookup;
    for (const Order& o : wave.orders)
        lookup[o.id] = &o;

    std::vector<const Order*> remaining;
    for (const Order& o : wave.orders)
        remaining.push_back(&o);

    std::vector<Route> routes;

    while (!remaining.empty()) {
        Route batch;
        batch.wave_id = wave.id;
        batch.list_id = list_id_counter++;

        // Eerste order als zaad
        batch.orders.push_back(remaining.front()->id);
        remaining.erase(remaining.begin());

        // Cache huidige locaties en afstand van de batch
        auto current_locs = collect_locs(batch.orders, lookup);
        double current_dist = compute_distance(dist_, current_locs);

        // Vul batch aan tot n_max
        while (static_cast<int>(batch.orders.size()) < n_max_ && !remaining.empty()) {
            double best_marginal = std::numeric_limits<double>::max();
            int    best_idx      = -1;

            for (int i = 0; i < static_cast<int>(remaining.size()); ++i) {
                // Voeg tijdelijk de kandidaat-locaties toe
                std::unordered_set<LocationID> seen(current_locs.begin(), current_locs.end());
                std::vector<LocationID> extended = current_locs;

                for (const LocationID& loc : remaining[i]->locations)
                    if (seen.insert(loc).second)
                        extended.push_back(loc);

                double new_dist   = compute_distance(dist_, extended);
                double marginal   = new_dist - current_dist;

                if (marginal < best_marginal) {
                    best_marginal = marginal;
                    best_idx      = i;
                }
            }

            // Voeg de beste kandidaat toe
            const Order* best = remaining[best_idx];
            batch.orders.push_back(best->id);

            // Update gecachede locaties en afstand
            std::unordered_set<LocationID> seen(current_locs.begin(), current_locs.end());
            for (const LocationID& loc : best->locations)
                if (seen.insert(loc).second)
                    current_locs.push_back(loc);
            current_dist = compute_distance(dist_, current_locs);

            remaining.erase(remaining.begin() + best_idx);
        }

        routes.push_back(std::move(batch));
    }

    return routes;
}

double GreedyBatcher::marginal_cost(const Route&, const Order&,
                                     const std::vector<Wave>&) const {
    return 0.0; // niet meer gebruikt, logica zit in batch_wave
}
