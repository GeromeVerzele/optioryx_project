#pragma once
#include "types.h"
#include <string>
#include <vector>

// Laadt pick_data.csv en bouwt de Wave/Order structuur op
//
// pick_data.csv kolommen:
//   WAVE_ID, ORDER_ID, LOCATION_ID, LIST_ID, ASIS_SEQUENCE
//
// Na het laden:
//   - Elke Wave bevat zijn Orders
//   - Elke Order bevat zijn locaties gesorteerd op ASIS_SEQUENCE
//   - Dubbele locaties binnen een order worden samengevoegd (uniek per lijst)
class DataLoader {
public:
    // Laad pick_data.csv; geeft gegroepeerde waves terug
    std::vector<Wave> load_picks(const std::string& filepath);

    // Laad example_submission.csv als baseline-oplossing
    // Handig om de beginscore te berekenen
    Solution load_submission(const std::string& filepath,
                             const std::vector<Wave>& waves);

    // Schrijf een oplossing als CSV naar het Kaggle-formaat:
    //   WAVE_ID, ORDER_ID, LIST_ID
    void write_submission(const std::string& filepath,
                          const Solution& sol,
                          const std::vector<Wave>& waves);
};
