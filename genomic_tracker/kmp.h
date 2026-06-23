#ifndef KMP_H
#define KMP_H

#include <vector>
#include <string>
using namespace std;

vector<int> buildLPS(string pattern);
vector<int> KMPsearch(string text, string pattern);

#endif