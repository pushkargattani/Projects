# Genomic Mutation Tracker

A high-performance DNA mutation detection engine built in C++ with MySQL analytics.

## What it does
Searches patient DNA sequences for known disease-causing mutation patterns using two classic string algorithms, computes patient risk scores, and stores results in MySQL for analytics.

## Tech Stack
- **C++** — core algorithms, data processing
- **Python** — MySQL database insertion
- **MySQL** — storage and SQL analytics

## Algorithms
- **KMP (Knuth-Morris-Pratt)** — single pattern search using LPS array, O(N+M)
- **Rabin-Karp** — multi-pattern search using rolling hash, groups patterns by length for efficiency

Both algorithms run on same data and results are cross-validated.

## Project Structure
genomic_tracker/

generator.cpp      → generates 100 simulated patients with age-correlated mutations

patient.h          → Patient struct definition

kmp.cpp / kmp.h    → KMP algorithm implementation

rabin_karp.cpp / rabin_karp.h → Rabin-Karp implementation

main.cpp           → orchestrates everything

db.cpp / db.h      → writes results to CSV

insert_db.py       → inserts CSV data into MySQL

setup.sql          → creates MySQL database and tables

## How to Run

### Step 1: Generate patient data
```bash
g++ generator.cpp -o generator
.\generator
```

### Step 2: Run mutation tracker
```bash
g++ main.cpp kmp.cpp rabin_karp.cpp db.cpp -o tracker
.\tracker
```

### Step 3: Setup MySQL
Run setup.sql in MySQL Workbench first, then:
```bash
pip install mysql-connector-python
python insert_db.py
```

## Key Features
- Age-correlated mutation injection (older patients get more mutations, 0 mutations possible for young healthy patients)
- KMP vs Rabin-Karp performance benchmarking with timing
- Global mutation frequency tracking using unordered_map + priority_queue (top-K mutations)
- Patient risk scoring based on mutation count, unique types, and density
- SQL window functions for age-group analytics and patient ranking

## Simulation Note
DNA data is synthetically generated. Mutation injection rate correlates with age to simulate real biological behaviour where older patients accumulate more DNA mutations due to increased cell division. In production, this would use real genomic data from NCBI or similar databases.

## SQL Analytics
- Top 10 highest risk patients
- Average mutations per age group  
- Most common mutations across all patients
- Window function: patient risk ranking with overall average
- Age group mutation density comparison

## Algorithm Comparison
KMP outperforms Rabin-Karp on short sequences (1000 chars) due to lower setup overhead. At genome scale (millions of characters, 50+ patterns), Rabin-Karp's O(1) rolling hash advantage would dominate. This tradeoff is demonstrated and benchmarked in the project.