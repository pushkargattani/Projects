#include <iostream>
#include <vector>
#include <string>
#include "kmp.h"
using namespace std;

// ═══════════════════════════════════════
// BUILD LPS ARRAY
// ═══════════════════════════════════════
vector<int> buildLPS(string pattern) {
    int m = pattern.length();
    vector<int> lps(m, 0);  // all zeros initially

    int i = 1;  // moves forward always
    int j = 0;  // tracks matching prefix length

    while (i < m) {
        if (pattern[i] == pattern[j]) {
            // match found
            lps[i] = j + 1;
            i++;
            j++;
        }
        else {
            // mismatch
            if (j > 0) {
                j = lps[j - 1];  // fall back, don't move i
            }
            else {
                lps[i] = 0;
                i++;
            }
        }
    }

    return lps;
}


// ═══════════════════════════════════════
// KMP SEARCH
// finds all positions where pattern
// appears in text
// 1 based indexing (Abdul Bari style)
// ═══════════════════════════════════════
vector<int> KMPsearch(string text, string pattern) {
    vector<int> positions;  // stores where pattern found

    // add dummy character at index 0
    // so real content starts at index 1
    text    = " " + text;
    pattern = " " + pattern;

    int n = text.length() - 1;     // real length of text
    int m = pattern.length() - 1;  // real length of pattern

    // build LPS on pattern (without dummy char)
    vector<int> lps = buildLPS(pattern.substr(1));

    int i = 1;  // moves on text
    int j = 0;  // moves on pattern

    while (i <= n) {
        if (text[i] == pattern[j + 1]) {
            // match
            i++;
            j++;
        }
        else {
            // mismatch
            if (j > 0) {
                j = lps[j - 1];  // fall back using LPS
            }
            else {
                i++;  // j already 0, just move i
            }
        }

        // pattern fully matched
        if (j == m) {
            // found at position i-m (1 based)
            positions.push_back(i - m);
            j = lps[j - 1];  // look for more occurrences
        }
    }

    return positions;
}


// // ═══════════════════════════════════════
// // MAIN - test it
// // ═══════════════════════════════════════
// int main() {
//     string dna     = "AABAABAAB";
//     string pattern = "AABAA";

//     cout << "DNA     : " << dna     << "\n";
//     cout << "Pattern : " << pattern << "\n\n";

//     // build and print LPS
//     vector<int> lps = buildLPS(pattern);
//     cout << "LPS array: ";
//     for (int x : lps) cout << x << " ";
//     cout << "\n\n";

//     // search
//     vector<int> positions = KMPsearch(dna, pattern);

//     if (positions.empty()) {
//         cout << "Pattern not found\n";
//     }
//     else {
//         cout << "Pattern found at positions: ";
//         for (int pos : positions) cout << pos << " ";
//         cout << "\n";
//         cout << "Total occurrences: " << positions.size() << "\n";
//     }

//     return 0;
// }