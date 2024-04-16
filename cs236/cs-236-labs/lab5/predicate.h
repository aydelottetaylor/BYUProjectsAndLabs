#pragma once
#include <string>

using namespace std;

class Predicate {
private:
    string name;

public:
    Predicate(const string& name) : name(name) {}

    string getName() const {
        return name;
    }
};