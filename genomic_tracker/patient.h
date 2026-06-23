#ifndef PATIENT_H
#define PATIENT_H

#include <string>
#include <vector>
#include <map>
using namespace std;

struct Patient {
    string id;
    int age;
    string dna;

    // KMP results
    int totalMutations;
    map<string, vector<int>> mutationMap;

    // Rabin-Karp results
    map<string, vector<int>> rkMutationMap;
    int rkTotalMutations;

    // statistics
    int uniqueMutations;
    double mutationDensity;
    double riskScore;
};

#endif