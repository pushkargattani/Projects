#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

// ═══════════════════════════════════════
// CONFIG
// ═══════════════════════════════════════
const int NUM_PATIENTS = 100;
const int DNA_LENGTH   = 1000;

string MUTATIONS[] = {"GGT", "ATCGG", "TTTCGA", "CGGA", "TAGG"};
const int NUM_MUTATIONS = 5;


// ═══════════════════════════════════════
// STEP 1: Generate random DNA string
// ═══════════════════════════════════════
string generateDNA(int length) {
    string bases = "ATCG";
    string dna = "";
    for (int i = 0; i < length; i++) {
        dna += bases[rand() % 4];
    }
    return dna;
}


// ═══════════════════════════════════════
// STEP 2: Inject one mutation into DNA
// ═══════════════════════════════════════
string injectMutation(string dna, string mutation) {
    int maxPos = dna.length() - mutation.length();
    if (maxPos <= 0) return dna;

    int pos = rand() % maxPos;
    for (int i = 0; i < mutation.length(); i++) {
        dna[pos + i] = mutation[i];
    }
    return dna;
}


// ═══════════════════════════════════════
// STEP 3: Decide mutations based on age
//
// Age 20-29 → 0 or 1       (mostly healthy)
// Age 30-49 → 0, 1 or 2    (some risk)
// Age 50-69 → 1, 2 or 3    (guaranteed at least 1)
// Age 70-80 → 2, 3 or 4    (high risk)
//
// Overlaps allowed — makes it realistic
// Same age can give different mutation counts
// Biologically: age influences probability,
// not a guaranteed fixed count
// ═══════════════════════════════════════
int mutationsForAge(int age) {
    if (age < 30)       return rand() % 2;       // 0 or 1
    else if (age < 50)  return rand() % 3;       // 0, 1 or 2
    else if (age < 70)  return 1 + rand() % 3;   // 1, 2 or 3
    else                return 2 + rand() % 3;   // 2, 3 or 4
}


// ═══════════════════════════════════════
// MAIN
// ═══════════════════════════════════════
int main() {
    srand(time(0));

    ofstream file("patients.txt");
    if (!file.is_open()) {
        cout << "Error: could not create patients.txt" << endl;
        return 1;
    }

    cout << "Generating " << NUM_PATIENTS << " patients...\n";

    for (int i = 1; i <= NUM_PATIENTS; i++) {

        // Patient ID: P001, P002 ... P100
        string id = "P";
        if (i < 10)       id += "00" + to_string(i);
        else if (i < 100) id += "0"  + to_string(i);
        else               id +=        to_string(i);

        // Random age 20 to 80
        int age = 20 + rand() % 61;

        // Generate base random DNA
        string dna = generateDNA(DNA_LENGTH);

        // Inject mutations based on age
        // 0 mutations possible for young patients
        // overlaps allowed for realism
        int numToInject = mutationsForAge(age);
        for (int j = 0; j < numToInject; j++) {
            string mutation = MUTATIONS[rand() % NUM_MUTATIONS];
            dna = injectMutation(dna, mutation);
        }

        // Write to file: P001,34,ATCGGTAGCT...
        file << id << "," << age << "," << dna << "\n";
    }

    file.close();

    cout << "Done! patients.txt created.\n";
    cout << "Patients        : " << NUM_PATIENTS << "\n";
    cout << "DNA length      : " << DNA_LENGTH << " chars per patient\n";
    cout << "Mutation patterns searched: " << NUM_MUTATIONS << "\n";

    return 0;
}