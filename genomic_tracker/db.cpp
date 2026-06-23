#include "db.h"
#include <iostream>
#include <fstream>
using namespace std;

void insertToMySQL(vector<Patient>& patients,
                   unordered_map<string, int>& freqMap)
{
    // ── Write patients CSV ──
    ofstream pFile("results.csv");
    pFile << "patient_id,age,dna_length,total_mutations,"
          << "unique_mutations,mutation_density,risk_score\n";

    for (auto& p : patients) {
        pFile << p.id              << ","
              << p.age             << ","
              << p.dna.length()    << ","
              << p.totalMutations  << ","
              << p.uniqueMutations << ","
              << p.mutationDensity << ","
              << p.riskScore       << "\n";
    }
    pFile.close();
    cout << "results.csv written\n";

    // ── Write mutation results CSV ──
    ofstream mFile("mutation_results.csv");
    mFile << "patient_id,mutation_name,position,algorithm\n";

    for (auto& p : patients) {
        for (auto& entry : p.mutationMap) {
            for (int pos : entry.second) {
                mFile << p.id        << ","
                      << entry.first << ","
                      << pos         << ","
                      << "KMP"       << "\n";
            }
        }
    }
    mFile.close();
    cout << "mutation_results.csv written\n";

    // ── Write frequencies CSV ──
    ofstream fFile("frequencies.csv");
    fFile << "mutation_name,total_count\n";

    for (auto& entry : freqMap) {
        fFile << entry.first  << ","
              << entry.second << "\n";
    }
}