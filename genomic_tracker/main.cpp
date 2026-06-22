#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;


// ═══════════════════════════════════════
// PATIENT STRUCT
// ═══════════════════════════════════════
struct Patient {
    string id;
    int age;
    string dna;
};


// ═══════════════════════════════════════
// LOAD PATIENTS FROM FILE
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
        p.id  = id;
        p.age = stoi(ageStr);
        p.dna = dna;

        patients.push_back(p);
    }

    file.close();
    return patients;
}


// ═══════════════════════════════════════
// MAIN
// ═══════════════════════════════════════
int main() {
    cout << "=== Genomic Mutation Tracker ===\n\n";

    vector<Patient> patients = loadPatients("patients.txt");

    if (patients.empty()) {
        cout << "No patients loaded. Run generator first.\n";
        return 1;
    }

    cout << "Loaded " << patients.size() << " patients\n\n";

    // Print first 5 to verify
    cout << "First 5 patients:\n";
    cout << "─────────────────────────────\n";
    for (int i = 0; i < 5 && i < patients.size(); i++) {
        cout << "ID  : " << patients[i].id  << "\n";
        cout << "Age : " << patients[i].age << "\n";
        cout << "DNA : " << patients[i].dna.substr(0, 50) << "...\n";
        cout << "─────────────────────────────\n";
    }

    return 0;
}