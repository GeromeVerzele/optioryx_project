#include "data_loader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <map>

// ---- Hulpfunctie: lees CSV-header en geef kolomindices terug ----
static std::map<std::string, int> parse_header(const std::string& line) {
    std::map<std::string, int> idx;
    std::stringstream ss(line);
    std::string col;
    int i = 0;
    while (std::getline(ss, col, ',')) {
        // verwijder \r als aanwezig (Windows line endings)
        if (!col.empty() && col.back() == '\r') col.pop_back();
        idx[col] = i++;
    }
    return idx;
}

// ---- load_picks ----
std::vector<Wave> DataLoader::load_picks(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("Kan bestand niet openen: " + filepath);

    std::string line;
    std::getline(file, line);
    auto col = parse_header(line);

    // Tussentijdse opslag: wave_id → order_id → {location, asis_seq}
    std::map<WaveID, std::map<OrderID, std::vector<std::pair<int, LocationID>>>> raw;

    while (std::getline(file, line)) {
        if (line.empty() || line == "\r") continue;
        std::stringstream ss(line);
        std::vector<std::string> fields;
        std::string f;
        while (std::getline(ss, f, ',')) fields.push_back(f);

        WaveID     wave   = std::stoi(fields[col.at("WAVE_ID")]);
        OrderID    order  = std::stoi(fields[col.at("ORDER_ID")]);
        LocationID loc    = fields[col.at("LOCATION_ID")];
        int        seq    = std::stoi(fields[col.at("ASIS_SEQUENCE")]);
        if (!loc.empty() && loc.back() == '\r') loc.pop_back();

        raw[wave][order].emplace_back(seq, loc);
    }

    // Bouw Wave-structuur
    std::vector<Wave> waves;
    for (auto& [wid, orders_map] : raw) {
        Wave wave;
        wave.id = wid;
        for (auto& [oid, picks] : orders_map) {
            // Sorteer op asis_sequence
            std::sort(picks.begin(), picks.end());

            Order order;
            order.id = oid;
            order.wave_id = wid;
            // Unieke locaties (volgorde behouden)
            for (auto& [seq, loc] : picks) {
                if (order.locations.empty() || order.locations.back() != loc)
                    order.locations.push_back(loc);
            }
            wave.orders.push_back(std::move(order));
        }
        waves.push_back(std::move(wave));
    }
    return waves;
}

// ---- write_submission ----
void DataLoader::write_submission(const std::string& filepath,
                                  const Solution& sol,
                                  const std::vector<Wave>& waves) {
    std::ofstream out(filepath);
    if (!out.is_open())
        throw std::runtime_error("Kan bestand niet schrijven: " + filepath);

    out << "WAVE_ID,ORDER_ID,LIST_ID\n";

    for (size_t wi = 0; wi < sol.size(); ++wi) {
        for (const Route& route : sol[wi]) {
            for (OrderID oid : route.orders) {
                out << route.wave_id << ','
                    << oid           << ','
                    << route.list_id << '\n';
            }
        }
    }
}
