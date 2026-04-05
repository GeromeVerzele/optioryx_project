#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include "types.h"
#include "distance.h"
#include "evaluator.h"
#include "data_loader.h"
#include "greedy/greedy.h"
// #include "simulated_annealing/sa.h"   // TODO
// #include "genetic/ga.h"               // TODO
// #include "seed_selection/seed_selection.h" // TODO

// ---- Eenvoudige CLI-argumentparser ----
struct Args {
    std::string algo;
    std::string input    = "data/pick_data.csv";
    std::string dist     = "data/dist_matrix.csv";
    std::string output   = "results/submission.csv";
    int         n_max    = N_MAX;
    // SA
    std::string cooling  = "geometric";
    std::string neighbor = "swap";
    std::string start    = "greedy";
    // GA
    int gen     = 500;
    int popsize = 100;
    // Seed-and-select
    std::string seed   = "farthest";
    std::string select = "overlap";
};

Args parse_args(int argc, char* argv[]) {
    Args a;
    if (argc < 2) { a.algo = "help"; return a; }
    a.algo = argv[1];
    for (int i = 2; i < argc - 1; i += 2) {
        std::string key = argv[i];
        std::string val = argv[i + 1];
        if      (key == "--input")   a.input   = val;
        else if (key == "--dist")    a.dist    = val;
        else if (key == "--output")  a.output  = val;
        else if (key == "--nmax")    a.n_max   = std::stoi(val);
        else if (key == "--cooling") a.cooling = val;
        else if (key == "--neighbor")a.neighbor= val;
        else if (key == "--start")   a.start   = val;
        else if (key == "--gen")     a.gen     = std::stoi(val);
        else if (key == "--popsize") a.popsize = std::stoi(val);
        else if (key == "--seed")    a.seed    = val;
        else if (key == "--select")  a.select  = val;
    }
    return a;
}

void print_usage(const char* prog) {
    std::cout << "Gebruik: " << prog << " <algo> [opties]\n\n"
              << "Algoritmen: greedy | sa | ga | seed\n\n"
              << "Gemeenschappelijk:\n"
              << "  --input  <pad>   pick_data.csv     (standaard: data/pick_data.csv)\n"
              << "  --dist   <pad>   dist_matrix.csv   (standaard: data/dist_matrix.csv)\n"
              << "  --output <pad>   submission output (standaard: results/submission.csv)\n"
              << "  --nmax   <int>   max orders/lijst  (standaard: 20)\n\n"
              << "SA:  --cooling geometric|linear  --neighbor swap|move  --start greedy|random\n"
              << "GA:  --gen <int>  --popsize <int>\n"
              << "Seed:--seed first|farthest|custom  --select overlap|minadd|custom\n";
}

int main(int argc, char* argv[]) {
    Args args = parse_args(argc, argv);

    if (args.algo == "help" || args.algo == "--help") {
        print_usage(argv[0]);
        return 0;
    }

    // -- Data laden --
    std::cout << "[1/3] Data laden...\n";
    DistanceMatrix dist;
    dist.load(args.dist);

    DataLoader loader;
    std::vector<Wave> waves = loader.load_picks(args.input);
    std::cout << "      " << waves.size() << " waves geladen.\n";

    Evaluator eval(dist);

    // -- Algoritme kiezen en uitvoeren --
    Solution sol;

    if (args.algo == "greedy") {
        std::cout << "[2/3] Greedy heuristiek uitvoeren...\n";
        GreedyBatcher batcher(dist, eval, args.n_max);
        sol = batcher.solve(waves);

    } else if (args.algo == "sa") {
        std::cout << "[2/3] Simulated Annealing uitvoeren...\n";
        // SAConfig cfg; ... (implementeer in sa.cpp)
        throw std::runtime_error("SA nog niet geïmplementeerd.");

    } else if (args.algo == "ga") {
        std::cout << "[2/3] Genetisch Algoritme uitvoeren...\n";
        throw std::runtime_error("GA nog niet geïmplementeerd.");

    } else if (args.algo == "seed") {
        std::cout << "[2/3] Seed-and-Selection uitvoeren...\n";
        throw std::runtime_error("Seed-and-selection nog niet geïmplementeerd.");

    } else {
        std::cerr << "Onbekend algoritme: " << args.algo << "\n";
        print_usage(argv[0]);
        return 1;
    }

    // -- Score berekenen en opslaan --
    std::cout << "[3/3] Resultaat opslaan...\n";
    double score = eval.total_distance(sol, waves);
    std::cout << "      Totale wandelafstand: " << score << "\n";

    loader.write_submission(args.output, sol, waves);
    std::cout << "      Opgeslagen in: " << args.output << "\n";

    return 0;
}
