#pragma once
#include <vector>
#include <string>
#include <unordered_map>

// ----- Basistypen (afgestemd op Kaggle-data) -----

using WaveID     = int;
using OrderID    = int;
using LocationID = std::string;  // bijv. "02-46", "START", "END"
using ListID     = int;

// Één pick = één rij uit pick_data.csv
struct Pick {
    WaveID     wave_id;
    OrderID    order_id;
    LocationID location_id;
    int        asis_sequence;   // sorteervolgorde binnen een route
};

// Een order = alle picks met hetzelfde ORDER_ID (binnen één wave)
struct Order {
    OrderID                  id;
    WaveID                   wave_id;
    std::vector<LocationID>  locations;  // unieke locaties, gesorteerd op asis_sequence
};

// Een lijst (route) = batch van orders die samen worden opgelopen
// Maximaal N_MAX orders per lijst
struct Route {
    ListID               list_id;
    WaveID               wave_id;
    std::vector<OrderID> orders;
};

// Een wave = alle orders met hetzelfde WAVE_ID
struct Wave {
    WaveID             id;
    std::vector<Order> orders;
};

// Volledige oplossing: voor elke wave een vector van routes
using Solution = std::vector<std::vector<Route>>;

// Constraint uit de probleemomschrijving
constexpr int N_MAX = 20;
