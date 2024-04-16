#pragma once
#include <vector>
#include <string>
#include <sstream>
#include "scheme.h"

using namespace std;

class Tuple : public vector<string> {

public:
    Tuple(vector<string> values) : vector<string>(values) { }

    string toString(const Scheme& scheme) const {
        const Tuple& tuple = *this;
        stringstream out;
        for (size_t i = 0; i < scheme.size(); ++i) {
            out << scheme.at(i) << "=" << tuple.at(i);
            if (i != scheme.size() -1) {
                out << ", ";
            }
        }
        
        return out.str();
    }
};