#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <iostream>
#include "scheme.h"
#include "tuple.h"

using namespace std;

class Relation {

private:
    string name;
    Scheme scheme;
    set <Tuple> tuples;

public:
    Relation () {}

    Relation(const string& name, const Scheme& scheme) 
        : name(name), scheme(scheme) { }

    void addTuple(const Tuple& tuple) {
        tuples.insert(tuple);
    }

    string toString() const {
        stringstream out;
        int i = 0;
        bool lastTuple = false;
        for (const auto& tuple: tuples) {
            ++i;
            if(i == tuples.size()) {
                lastTuple = true;
            }
            out << tuple.toString(scheme);
            if(!lastTuple) {
                out << endl;
            }
        }
        return out.str();
    }

    Relation select(int index, const string& value) const {
        Relation result(name, scheme);
        for (const auto& tuple : tuples) {
            if(tuple.at(index) == value) {
                result.addTuple(tuple);
            }
        }
        return result;
    }

    Relation selectTuplesWithSame(const set<int>& attributeIndices) const {
        Relation result(name, scheme);
        for (const auto& tuple : tuples) {
            bool allSame = true;
            int firstIndex = *attributeIndices.begin();
            for (const auto& index : attributeIndices) {
                if (tuple[index] != tuple[firstIndex]) {
                    allSame = false;
                    break;
                }
            }
            if (allSame) {
                result.addTuple(tuple);
            }
        }
        return result;
    }

    Relation selectTuplesWithDifferent(const set<int>& attributeIndices) const {
        Relation result(name, scheme);
        for (const auto& tuple : tuples) {
            bool allDifferent = true;
            set<string> uniqueValues;
            for (const auto& index : attributeIndices) {
                if (uniqueValues.find(tuple[index]) != uniqueValues.end()) {
                    allDifferent = false;
                    break;
                } else {
                    uniqueValues.insert(tuple[index]);
                }
            }
            if (allDifferent) {
                result.addTuple(tuple);
            }
        }
        return result;
    }

    Relation rename(const vector<string>& newAttributes) const {
        Relation result(name, newAttributes);
        for (const auto& tuple : tuples) {
            Tuple newTuple = tuple;
            auto data = tuple.getData(scheme);
            for (const auto& attribute : newAttributes) {
                string value = data[attribute];
                newTuple.addAttribute(value);
            }
            result.addTuple(newTuple);
        }
        return result;
    }

    Relation renameSingleAttribute(const string& newName, int index) const {
        Scheme newScheme = scheme;
        if (!newScheme.empty()) {
            newScheme[index] = newName;
            Relation result(name, newScheme);
            for (const auto& tuple : tuples) {
                result.addTuple(tuple);
            }
            return result;
        } else {
            // Handle the case where the scheme is empty
            return Relation(name, scheme);
        }
    }

    Relation project(const std::vector<std::string>& projectedAttributes) const {
        Relation result(name, projectedAttributes);
        for (const auto& tuple : tuples) {
            Tuple newTuple;
            newTuple.setProjectedAttributes(projectedAttributes, tuple, scheme);
            result.addTuple(newTuple);
        }
        return result;
    }

    string getName() const {
        return name;
    }

    Scheme getAttributes() const {
        return scheme;
    }

    set<Tuple> getTuples() const {
        return tuples;
    }
};