#include "data_loader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <map>
#include <iostream>

static std::string trim(std::string s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' '))
        s.pop_back();
    size_t start = s.find_first_not_of(" \t");
    if (start == std::string::npos) return "";
    return s.substr(start);
}

static std::map<std::string, int> parse_header(const std::string& line) {
    std::map<std::string, int> idx;
    std::stringstream ss(line);
    std::string col;
    int i = 0;
    while (std::getline(ss, col, ','))
        idx[trim(col)] = i++;
    return idx;
}

static std::vector<std::string> split_line(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string f;
    while (std::getline(ss, f, ','))
        fields.push_back(trim(f));
    return fields;
}

static int safe_int(const std::string& s, const std::string& ctx = "") {
    if (s.empty()) return -1;
    char* end = nullptr;
    long val = std::strtol(s.c_str(), &end, 10);
    if (end == s.c_str()) return -1;
    return static_cast<int>(val);
}

std::vector<Wave> DataLoader::load_picks(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("Kan bestand niet openen: " + filepath);

    std::string line;
    std::getline(file, line);
    auto col = parse_header(line);

    std::cout << "      Kolommen: ";
    for (auto& [k, v] : col) std::cout << k << "(" << v << ") ";
    std::cout << "\n";

    for (const char* req : {"WAVE_ID", "ORDER_ID", "LOCATION_ID", "ASIS_SEQUENCE"}) {
        if (col.find(req) == col.end())
            throw std::runtime_error(std::string("Kolom ontbreekt: ") + req);
    }

    int ci_wave = col.at("WAVE_ID");
    int ci_ord  = col.at("ORDER_ID");
    int ci_loc  = col.at("LOCATION_ID");
    int ci_seq  = col.at("ASIS_SEQUENCE");
    int ci_max  = std::max({ci_wave, ci_ord, ci_loc, ci_seq});

    // wave_id (string) → order_id (string) → picks
    std::map<WaveID, std::map<OrderID, std::vector<std::pair<int, LocationID>>>> raw;
    int row_num = 1, skipped = 0;

    while (std::getline(file, line)) {
        ++row_num;
        line = trim(line);
        if (line.empty()) continue;

        auto f = split_line(line);
        if (static_cast<int>(f.size()) <= ci_max) { ++skipped; continue; }

        WaveID     wave  = f[ci_wave];   // bijv. "W000012"
        OrderID    order = f[ci_ord];    // bijv. "O000010229"
        LocationID loc   = f[ci_loc];
        int        seq   = safe_int(f[ci_seq], "ASIS_SEQUENCE");

        if (wave.empty() || order.empty() || loc.empty() || seq < 0) {
            ++skipped; continue;
        }

        raw[wave][order].emplace_back(seq, loc);
    }

    std::cout << "      " << row_num << " rijen gelezen, "
              << skipped << " overgeslagen.\n";

    std::vector<Wave> waves;
    for (auto& [wid, orders_map] : raw) {
        Wave wave;
        wave.id = wid;
        for (auto& [oid, picks] : orders_map) {
            std::sort(picks.begin(), picks.end());
            Order order;
            order.id = oid;
            order.wave_id = wid;
            for (auto& [seq, loc] : picks)
                if (order.locations.empty() || order.locations.back() != loc)
                    order.locations.push_back(loc);
            wave.orders.push_back(std::move(order));
        }
        waves.push_back(std::move(wave));
    }
    std::cout << "      " << waves.size() << " waves opgebouwd.\n";
    return waves;
}

void DataLoader::write_submission(const std::string& filepath,
                                  const Solution& sol,
                                  const std::vector<Wave>&) {
    std::ofstream out(filepath);
    if (!out.is_open())
        throw std::runtime_error("Kan bestand niet schrijven: " + filepath);

    out << "WAVE_ID,ORDER_ID,LIST_ID\n";
    for (const auto& wave_routes : sol)
        for (const Route& route : wave_routes)
            for (const OrderID& oid : route.orders)
                out << route.wave_id << ',' << oid << ',' << route.list_id << '\n';
}
