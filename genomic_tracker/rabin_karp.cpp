#include "rabin_karp.h"
#include <iostream>
#include <map>
using namespace std;

// ═══════════════════════════════════════
// CHARACTER VALUE
// A=1, T=2, C=3, G=4
// ═══════════════════════════════════════
int charVal(char c) {
    if (c == 'A') return 1;
    if (c == 'T') return 2;
    if (c == 'C') return 3;
    if (c == 'G') return 4;
    return 0;
}


// ═══════════════════════════════════════
// COMPUTE HASH
// ═══════════════════════════════════════
long long computeHash(string s, int len) {
    long long hash  = 0;
    long long power = 1;

    for (int i = 0; i < len - 1; i++) power *= 4;

    for (int i = 0; i < len; i++) {
        hash  += charVal(s[i]) * power;
        power /= 4;
    }
    return hash;
}


// ═══════════════════════════════════════
// RABIN KARP MULTI PATTERN SEARCH
// groups patterns by length
// one pass per unique length
// ═══════════════════════════════════════
map<string, vector<int>> RabinKarpMulti(string text, vector<string> patterns) {
    map<string, vector<int>> results;
    int n = text.length();

    // ── Step 1: group patterns by length ──
    // lengthGroups[3] = {"GGT"}
    // lengthGroups[4] = {"CGGA", "TAGG"}
    // lengthGroups[5] = {"ATCGG"}
    // lengthGroups[6] = {"TTTCGA"}
    map<int, vector<string>> lengthGroups;
    for (string& p : patterns) {
        lengthGroups[p.length()].push_back(p);
    }

    // ── Step 2: one pass per unique length ──
    for (auto& group : lengthGroups) {
        int m = group.first;                    // window size
        vector<string>& pats = group.second;    // patterns of this length

        if (m > n) continue;

        // compute hash for ALL patterns of this length
        map<long long, vector<string>> hashToPattern;
        for (string& p : pats) {
            long long h = computeHash(p, m);
            hashToPattern[h].push_back(p);      // multiple patterns can have same hash
        }

        // compute hash of first window
        long long winHash = computeHash(text.substr(0, m), m);

        // compute base^(m-1) for rolling
        long long power = 1;
        for (int i = 0; i < m - 1; i++) power *= 4;

        // ── ONE PASS through text for this length ──
        for (int i = 0; i <= n - m; i++) {

            // check if window hash matches any pattern hash
            if (hashToPattern.count(winHash)) {

                // get actual window string
                string window = text.substr(i, m);

                // check against all patterns with this hash
                for (string& p : hashToPattern[winHash]) {
                    if (window == p) {
                        results[p].push_back(i + 1);  // 1 based
                    }
                    // else spurious hit, ignore
                }
            }

            // roll hash to next window
            if (i < n - m) {
                winHash = (winHash - charVal(text[i]) * power)
                          * 4
                          + charVal(text[i + m]);
            }
        }
    }

    return results;
}