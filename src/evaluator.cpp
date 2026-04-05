#include "evaluator.h"
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

Evaluator::Evaluator(const DistanceMatrix& dist) : dist_(dist) {}

// Verzamel alle unieke locaties van een route, gesorteerd op asis_sequence.
// Omdat Order::locations al gesorteerd is, hoeven we alleen samen te voegen
// en duplicaten te verwijderen (locaties die in meerdere orders voorkomen
// tellen slechts één keer).
std::vector<LocationID> Evaluator::collect_locations(
    const Route& route, const std::vector<Wave>& waves) const
{
    // Bouw een lookup: order_id → Order
    // (kleine overhead; voor performantie kan dit later gecached worden)
    std::unordered_map<OrderID, const Order*> order_map;
    for (const Wave& w : waves)
        for (const Order& o : w.orders)
            order_map[o.id] = &o;

    // Verzamel unieke locaties; bewaar volgorde via set + vector
    std::unordered_set<LocationID> seen;
    std::vector<std::pair<int, LocationID>> loc_seq; // (asis_seq, loc)

    for (OrderID oid : route.orders) {
        const Order* o = order_map.at(oid);
        for (size_t i = 0; i < o->locations.size(); ++i) {
            const LocationID& loc = o->locations[i];
            if (seen.insert(loc).second)
                loc_seq.emplace_back(static_cast<int>(i), loc);
            // Noot: voor een correcte ASIS_SEQUENCE-sortering over orders heen
            // is de echte seq-waarde nodig. Die wordt bij verfijning meegegeven.
        }
    }

    // Sorteer op invoegvolgorde (al gesorteerd per order; globale sortering
    // vereist de echte ASIS_SEQUENCE integers — te verfijnen zodra data beschikbaar)
    std::vector<LocationID> result;
    result.reserve(loc_seq.size());
    for (auto& [_, loc] : loc_seq) result.push_back(loc);
    return result;
}

double Evaluator::route_distance(const Route& route,
                                  const std::vector<Wave>& waves) const {
    auto locs = collect_locations(route, waves);
    return dist_.route_distance(locs);
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
                          const std::vector<Wave>& waves) const {
    bool ok = true;
    for (const auto& wave_routes : sol) {
        for (const Route& r : wave_routes) {
            if (static_cast<int>(r.orders.size()) > N_MAX) {
                std::cerr << "[FOUT] Lijst " << r.list_id
                          << " heeft " << r.orders.size()
                          << " orders (max " << N_MAX << ")\n";
                ok = false;
            }
        }
    }
    return ok;
}
