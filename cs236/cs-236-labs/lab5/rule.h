#pragma once
#include <string>
#include <vector>
#include "predicate.h"

using namespace std;

class Rule {
private:
    Predicate head;
    vector<Predicate> body;

public:
    Rule(const Predicate& head) : head(head) {}

    void addBodyPredicate(const Predicate& predicate) {
        body.push_back(predicate);
    }

    Predicate getHead() const {
        return head;
    }

    vector<Predicate> getBody() const {
        return body;
    }
};