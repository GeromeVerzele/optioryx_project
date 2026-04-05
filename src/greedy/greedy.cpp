#include "greedy/greedy.h"
#include <algorithm>
#include <limits>
#include <vector>

GreedyBatcher::GreedyBatcher(const DistanceMatrix& dist,
                             const Evaluator& eval,
                             int n_max)
    : dist_(dist), eval_(eval), n_max_(n_max) {}

Solution GreedyBatcher::solve(const std::vector<Wave>& waves) const {
    Solution sol;
    int list_id = 1;
    for (const Wave& w : waves)
        sol.push_back(batch_wave(w, list_id));
    return sol;
}

std::vector<Route> GreedyBatcher::batch_wave(const Wave& wave,
                                              int& list_id_counter) const {
    std::vector<const Order*> remaining;
    for (const Order& o : wave.orders)
        remaining.push_back(&o);

    // Hulp-wave met één wave voor de evaluator
    std::vector<Wave> wrap = {wave};

    std::vector<Route> routes;

    while (!remaining.empty()) {
        Route batch;
        batch.wave_id = wave.id;
        batch.list_id = list_id_counter++;

        // Eerste order: neem de eerste resterende order als zaad
        batch.orders.push_back(remaining.front()->id);
        remaining.erase(remaining.begin());

        // Vul de batch aan tot n_max
        while (static_cast<int>(batch.orders.size()) < n_max_ && !remaining.empty()) {
            double best_cost = std::numeric_limits<double>::max();
            int    best_idx  = -1;

            for (int i = 0; i < static_cast<int>(remaining.size()); ++i) {
                double cost = marginal_cost(batch, *remaining[i], wrap);
                if (cost < best_cost) {
                    best_cost = cost;
                    best_idx  = i;
                }
            }

            batch.orders.push_back(remaining[best_idx]->id);
            remaining.erase(remaining.begin() + best_idx);
        }

        routes.push_back(std::move(batch));
    }

    return routes;
}

double GreedyBatcher::marginal_cost(const Route& route,
                                     const Order& candidate,
                                     const std::vector<Wave>& waves) const {
    double before = eval_.route_distance(route, waves);

    // Maak tijdelijke route met de candidate erbij
    Route extended = route;
    extended.orders.push_back(candidate.id);

    double after = eval_.route_distance(extended, waves);
    return after - before;
}
