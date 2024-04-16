#pragma once
#include <vector>
#include <string>

using namespace std;

class Scheme : public vector<string> {

public:
    Scheme(const vector<string>& names) : vector<string>(names) { }

    // TODO: add more delagation functions as needed

};