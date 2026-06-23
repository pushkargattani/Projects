#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <unordered_map>
#include <queue>
#include <chrono>
#include "patient.h"
#include "kmp.h"
#include "rabin_karp.h"
#include "db.h"
using namespace std;


// // ═══════════════════════════════════════
// // PATIENT STRUCT
// // ═══════════════════════════════════════
// struct Patient {
//     string id;
//     int age;
//     string dna;

//     // KMP results
//     int totalMutations;
//     map<string, vector<int>> mutationMap;

//     // Rabin-Karp results
//     map<string, vector<int>> rkMutationMap;
//     int rkTotalMutations;

//     // statistics
//     int uniqueMutations;
//     double mutationDensity;
//     double riskScore;
// };


// ═══════════════════════════════════════
// LOAD PATIENTS
// ═══════════════════════════════════════
vector<Patient> loadPatients(string filename) {
    vector<Patient> patients;

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: could not open " << filename << endl;
        return patients;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string id, ageStr, dna;

        getline(ss, id,     ',');
        getline(ss, ageStr, ',');
        getline(ss, dna,    '\n');

        Patient p;
        p.id               = id;
        p.age              = stoi(ageStr);
        p.dna              = dna;
        p.totalMutations   = 0;
        p.rkTotalMutations = 0;
        p.uniqueMutations  = 0;
        p.mutationDensity  = 0.0;
        p.riskScore        = 0.0;

        patients.push_back(p);
    }

    file.close();
    return patients;
}


// ═══════════════════════════════════════
// COMPUTE PATIENT STATISTICS + RISK SCORE
// ═══════════════════════════════════════
void computeStats(Patient& p) {
    // unique mutations = number of different types
    p.uniqueMutations = p.mutationMap.size();

    // density = mutations per 1000 chars
    p.mutationDensity = (double)p.totalMutations
                        / p.dna.length()
                        * 1000.0;

    // risk score formula
    p.riskScore = (p.totalMutations * 0.5)
                + (p.uniqueMutations * 1.0)
                + (p.mutationDensity * 0.3);
}


// ═══════════════════════════════════════
// COMPUTE GLOBAL MUTATION FREQUENCIES
// counts how many times each mutation
// appears across ALL patients
// ═══════════════════════════════════════
void computeFrequencies(
    vector<Patient>& patients,
    unordered_map<string, int>& freqMap)
{
    for (auto& p : patients) {
        for (auto& entry : p.mutationMap) {
            // entry.first  = mutation name "GGT"
            // entry.second = positions vector
            freqMap[entry.first] += entry.second.size();
        }
    }
}


// ═══════════════════════════════════════
// GET TOP K MUTATIONS USING PRIORITY QUEUE
// ═══════════════════════════════════════
void printTopK(unordered_map<string, int>& freqMap, int k) {

    // priority queue of pairs (frequency, name)
    // by default max heap → highest frequency on top
    priority_queue<pair<int,string>> pq;

    // push all mutations into priority queue
    for (auto& entry : freqMap) {
        pq.push({entry.second, entry.first});
        // {45, "GGT"}, {67, "TTTCGA"} etc
    }

    cout << "\n================================\n";
    cout << "   TOP " << k << " MOST COMMON MUTATIONS  \n";
    cout << "================================\n";

    int rank = 1;
    while (!pq.empty() && rank <= k) {
        auto top = pq.top();
        pq.pop();
        cout << rank << ". "
             << top.second          // mutation name
             << " -> "
             << top.first           // frequency
             << " occurrences across all patients\n";
        rank++;
    }
}


// ═══════════════════════════════════════
// PRINT MUTATION MAP
// ═══════════════════════════════════════
void printMutations(map<string, vector<int>>& mutMap) {
    if (mutMap.empty()) {
        cout << "  none\n";
        return;
    }
    for (auto& entry : mutMap) {
        cout << "  " << entry.first << " found at: ";
        for (int pos : entry.second) cout << pos << " ";
        cout << "\n";
    }
}


// ═══════════════════════════════════════
// MAIN
// ═══════════════════════════════════════
int main() {
    cout << "=== Genomic Mutation Tracker ===\n\n";

    vector<string> mutations = {"GGT", "ATCGG", "TTTCGA", "CGGA", "TAGG"};

    // load patients
    vector<Patient> patients = loadPatients("patients.txt");
    if (patients.empty()) {
        cout << "No patients loaded. Run generator first.\n";
        return 1;
    }
    cout << "Loaded " << patients.size() << " patients\n\n";


    // ── KMP search with timing ──
    auto kmpStart = chrono::high_resolution_clock::now();

    for (auto& p : patients) {
        for (string& mut : mutations) {
            vector<int> positions = KMPsearch(p.dna, mut);
            if (!positions.empty()) {
                p.totalMutations += positions.size();
                p.mutationMap[mut] = positions;
            }
        }
    }

    auto kmpEnd    = chrono::high_resolution_clock::now();
    double kmpTime = chrono::duration<double, milli>(kmpEnd - kmpStart).count();


    // ── Rabin-Karp search with timing ──
    auto rkStart = chrono::high_resolution_clock::now();

    for (auto& p : patients) {
        p.rkMutationMap = RabinKarpMulti(p.dna, mutations);
        for (auto& entry : p.rkMutationMap) {
            p.rkTotalMutations += entry.second.size();
        }
    }

    auto rkEnd    = chrono::high_resolution_clock::now();
    double rkTime = chrono::duration<double, milli>(rkEnd - rkStart).count();


    // ── compute statistics and risk scores ──
    for (auto& p : patients) {
        computeStats(p);
    }

    // ── compute global mutation frequencies ──
    unordered_map<string, int> freqMap;
    computeFrequencies(patients, freqMap);


    // ── timing comparison ──
    cout << "================================\n";
    cout << "     ALGORITHM PERFORMANCE      \n";
    cout << "================================\n";
    cout << "KMP time       : " << kmpTime << " ms\n";
    cout << "Rabin-Karp time: " << rkTime  << " ms\n";
    cout << "Faster by      : "
         << (kmpTime > rkTime ? kmpTime/rkTime : rkTime/kmpTime)
         << "x\n";
    cout << "Winner         : "
         << (kmpTime < rkTime ? "KMP" : "Rabin-Karp") << "\n";
    cout << "================================\n\n";


    // ── print top 5 most common mutations ──
    printTopK(freqMap, 5);


    // ── print first 5 patients ──
    cout << "\nResults - First 5 patients:\n";
    for (int i = 0; i < 5 && i < patients.size(); i++) {
        cout << "==================================\n";
        cout << "ID                   : " << patients[i].id               << "\n";
        cout << "Age                  : " << patients[i].age              << "\n";
        cout << "Total mutations      : " << patients[i].totalMutations   << "\n";
        cout << "Unique mutation types: " << patients[i].uniqueMutations  << "\n";
        cout << "Mutation density     : " << patients[i].mutationDensity  << " per 1000 chars\n";
        cout << "Risk Score           : " << patients[i].riskScore        << "\n";

        cout << "\nKMP Results:\n";
        printMutations(patients[i].mutationMap);

        cout << "\nRabin-Karp Results:\n";
        printMutations(patients[i].rkMutationMap);

        cout << "\nBoth algorithms match: "
             << (patients[i].totalMutations == patients[i].rkTotalMutations
                 ? "YES" : "NO")
             << "\n";
    }

    insertToMySQL(patients, freqMap);

    return 0;
}