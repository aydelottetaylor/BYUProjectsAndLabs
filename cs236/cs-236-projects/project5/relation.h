#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <iostream>
#include <algorithm>

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

    // Prints out relation
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

    // Performs union operation on two relations
    void Union(Relation& resultRelation) {
        Scheme scheme = getAttributes();
        for(auto& tuple : resultRelation.getTuples()) {
            int beforeAdd = getTuples().size();
            
            addTuple(tuple);
            
            if(getTuples().size() > beforeAdd) {
                cout << tuple.toString(scheme) << endl;
            }
        }
    }

    // Performs join operation on a right and a left relation, returns result relation
    Relation join(const Relation& right) {
        const Relation& left = *this;
        Relation result;

        Scheme combinedScheme = left.scheme;
        for (const auto& attr : right.scheme) {
            if (find(combinedScheme.begin(), combinedScheme.end(), attr) == combinedScheme.end()) {
                combinedScheme.push_back(attr);
            }
        }

        result.scheme = combinedScheme;

        for (const auto& leftTuple : left.tuples) {
            for (const auto& rightTuple : right.tuples) {
                if (joinable(left.scheme, right.scheme, leftTuple, rightTuple)) {
                    Tuple combinedTuple;
                    for (const auto& attr : combinedScheme) {
                        auto leftIt = find_if(left.scheme.begin(), left.scheme.end(), [&attr](const auto& elem) { return elem == attr; });
                        auto rightIt = find_if(right.scheme.begin(), right.scheme.end(), [&attr](const auto& elem) { return elem == attr; });
                        if (leftIt != left.scheme.end()) {
                            combinedTuple.push_back(leftTuple[distance(left.scheme.begin(), leftIt)]);
                        } else if (rightIt != right.scheme.end()) {
                            combinedTuple.push_back(rightTuple[distance(right.scheme.begin(), rightIt)]);
                        }
                    }
                    result.tuples.insert(combinedTuple);
                }
            }
        }
        return result;
    }

    // Test to see if two tuples are joinable based off of their schemes and values
    static bool joinable(const Scheme& leftScheme, const Scheme& rightScheme, const Tuple& leftTuple, const Tuple& rightTuple) {
        for (unsigned leftIndex = 0; leftIndex < leftScheme.size(); leftIndex++) {
            const string& leftName = leftScheme.at(leftIndex);
            const string& leftValue = leftTuple.at(leftIndex);
            for (unsigned rightIndex = 0; rightIndex < rightScheme.size(); rightIndex++) {
                const string& rightName = rightScheme.at(rightIndex);
                const string& rightValue = rightTuple.at(rightIndex);
            }
        }
        for (size_t i = 0; i < leftScheme.size(); ++i) {
            for (size_t j = 0; j < rightScheme.size(); ++j) {
                if(leftScheme.at(i) == rightScheme.at(j)) {
                    if(leftTuple.at(i) != rightTuple.at(j)) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    // Perform select operation on relaion, return result relation
    Relation select(int index, const string& value) const {
        Relation result(name, scheme);
        for (const auto& tuple : tuples) {
            if(tuple.at(index) == value) {
                result.addTuple(tuple);
            }
        }
        return result;
    }

    // Finds tuples with same attributes at a given indices, returns result relation with those tuples
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

    // Finds tuples with different attributes at given indices, returns relation with those tuples
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

    // Performs rename operation on a relation, returns new relation
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

    // Renames a single attribute in a given relation at a specific index 
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

    // Performs project operation on a relation given a vector of attributes to project
    Relation project(const vector<string>& projectedAttributes) const {
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