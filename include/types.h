#pragma once
#include <vector>
#include <string>

// ----- Basistypen -----
// WAVE_ID  = string, bijv. "W000012"
// ORDER_ID = string, bijv. "O000010229"

using WaveID     = std::string;
using OrderID    = std::string;
using LocationID = std::string;
using ListID     = int;

struct Order {
    OrderID                  id;
    WaveID                   wave_id;
    std::vector<LocationID>  locations;  // gesorteerd op ASIS_SEQUENCE
};

struct Route {
    ListID               list_id;
    WaveID               wave_id;
    std::vector<OrderID> orders;
};

struct Wave {
    WaveID             id;
    std::vector<Order> orders;
};

using Solution = std::vector<std::vector<Route>>;

constexpr int N_MAX = 20;
