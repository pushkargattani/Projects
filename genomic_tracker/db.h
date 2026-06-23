#ifndef DB_H
#define DB_H

#include "patient.h"
#include <unordered_map>
using namespace std;

void insertToMySQL(vector<Patient>& patients,
                   unordered_map<string, int>& freqMap);

#endif