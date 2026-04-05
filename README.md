# Optioryx – Order Batching Optimalisatie

Individuele opdracht: optimalisatie van picking-opdrachten via metaheuristieken.

## Projectstructuur

```
optioryx/
├── CMakeLists.txt
├── README.md
├── data/                          # Kaggle-data (niet ingecheckt)
│   ├── pick_data.csv              # 30.291 rijen: WAVE_ID, ORDER_ID, LOCATION_ID, LIST_ID, ASIS_SEQUENCE
│   ├── dist_matrix.csv            # 422×422 afstandsmatrix
│   └── example_submission.csv     # Baseline (random clustering)
├── results/                       # Output-submissies
├── include/
│   ├── types.h                    # Order, Wave, Route, Solution, N_MAX=20
│   ├── distance.h                 # DistanceMatrix (laadt dist_matrix.csv)
│   ├── evaluator.h                # Totale score + validatie
│   └── data_loader.h              # Laad pick_data.csv, schrijf submission.csv
├── src/
│   ├── main.cpp                   # CLI: kies algoritme + opties
│   ├── data_loader.cpp
│   ├── distance.cpp
│   ├── evaluator.cpp
│   ├── greedy/                    # Deel 1.1 – Greedy (cheapest insertion)
│   ├── simulated_annealing/       # Deel 1.2 – SA (2 koelschema's × 2 buren × 2 starts)
│   ├── genetic/                   # Deel 1.3 – GA
│   └── seed_selection/            # Deel 2   – Seed-and-selection
└── analysis/
    └── plot_results.py            # Python: convergentieplot, vergelijkingstabellen
```

## Bouwen

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Uitvoeren

```bash
cd build

# Greedy (volledig geïmplementeerd)
./optioryx greedy --input ../data/pick_data.csv --dist ../data/dist_matrix.csv --output ../results/greedy.csv

# Simulated Annealing
./optioryx sa --input ../data/pick_data.csv --dist ../data/dist_matrix.csv \
              --cooling geometric --neighbor swap --start greedy

# Genetisch Algoritme
./optioryx ga --input ../data/pick_data.csv --dist ../data/dist_matrix.csv \
              --gen 500 --popsize 100

# Seed-and-Selection
./optioryx seed --input ../data/pick_data.csv --dist ../data/dist_matrix.csv \
                --seed farthest --select overlap
```

## Scoreberekening (zoals Kaggle)

Per lijst:
1. Verzamel alle **unieke** picklocaties van de toegewezen orders
2. Sorteer op `ASIS_SEQUENCE`
3. Bereken `START → locaties → END` via `dist_matrix.csv`

Eindscore = som over alle lijsten (lager = beter).

## Data

| Bestand | Rijen | Beschrijving |
|---------|-------|--------------|
| pick_data.csv | 30.291 | Historische picks: WAVE_ID, ORDER_ID, LOCATION_ID, ASIS_SEQUENCE |
| dist_matrix.csv | 422×422 | Afstanden tussen locaties incl. START/END (niet symmetrisch) |
| example_submission.csv | 10.243 | Baseline: WAVE_ID, ORDER_ID, LIST_ID |

- 12 unieke waves · 10.246 unieke orders · 422 locaties · max **20 orders/lijst**

## Constraints (automatisch gevalideerd door Kaggle)

- Elke ORDER_ID precies één keer
- ORDER_ID behoudt zijn originele WAVE_ID
- Max 20 orders per LIST_ID
- Eén wave per lijst

## Deadlines

| Deadline | Datum | Wat |
|----------|-------|-----|
| Deadline 1 | ma 20 april 23u55 | Minstens 1 Kaggle-indiening |
| Deadline 2 | zo 24 mei 23u55 | Beste resultaat + verslag (max 5 A4, NL) + code via Ufora |
