#include "evaluator.h"
#include <unordered_set>
#include <unordered_map>
#include <iostream>

Evaluator::Evaluator(const DistanceMatrix& dist) : dist_(dist) {}

std::vector<LocationID> Evaluator::collect_locations(
    const Route& route, const std::vector<Wave>& waves) const
{
    // Bouw order-lookup
    std::unordered_map<OrderID, const Order*> order_map;
    for (const Wave& w : waves)
        for (const Order& o : w.orders)
            order_map[o.id] = &o;

    std::unordered_set<LocationID> seen;
    std::vector<LocationID> result;

    for (const OrderID& oid : route.orders) {
        auto it = order_map.find(oid);
        if (it == order_map.end()) continue;
        for (const LocationID& loc : it->second->locations)
            if (seen.insert(loc).second)
                result.push_back(loc);
    }
    return result;
}

double Evaluator::route_distance(const Route& route,
                                  const std::vector<Wave>& waves) const {
    return dist_.route_distance(collect_locations(route, waves));
}

double Evaluator::total_distance(const Solution& sol,
                                  const std::vector<Wave>& waves) const {
    double total = 0.0;
    for (const auto& wave_routes : sol)
        for (const Route& r : wave_routes)
            total += route_distance(r, waves);
    return total;
}

bool Evaluator::is_valid(const Solution& sol,
                          const std::vector<Wave>&) const {
    bool ok = true;
    for (const auto& wave_routes : sol)
        for (const Route& r : wave_routes)
            if (static_cast<int>(r.orders.size()) > N_MAX) {
                std::cerr << "[FOUT] Lijst " << r.list_id
                          << " heeft " << r.orders.size() << " orders (max " << N_MAX << ")\n";
                ok = false;
            }
    return ok;
}
