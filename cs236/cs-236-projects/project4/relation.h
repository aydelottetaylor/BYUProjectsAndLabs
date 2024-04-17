#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <iostream>
#include <algorithm>
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

    Relation join(const Relation& right) {
        const Relation& left = *this;
        Relation result;

        // Combine schemes
        Scheme combinedScheme = left.scheme;
        for (const auto& attr : right.scheme) {
            // Check if attribute from right relation is not present in combined scheme
            if (find(combinedScheme.begin(), combinedScheme.end(), attr) == combinedScheme.end()) {
                // Add attribute from right relation to combined scheme
                combinedScheme.push_back(attr);
            }
        }

        // Set the combined scheme to the result relation
        result.scheme = combinedScheme;

        // Iterate over each tuple in the first relation
        for (const auto& leftTuple : left.tuples) {
            // Iterate over each tuple in the second relation
            for (const auto& rightTuple : right.tuples) {
                // Check if tuples are joinable
                if (joinable(left.scheme, right.scheme, leftTuple, rightTuple)) {
                    // Combine tuples and add to result relation
                    Tuple combinedTuple;
                    for (const auto& attr : combinedScheme) {
                        // Find the corresponding value for the attribute in left or right tuple
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
            // cout << "left name: " << leftName << " value: " << leftValue << endl;
            for (unsigned rightIndex = 0; rightIndex < rightScheme.size(); rightIndex++) {
                const string& rightName = rightScheme.at(rightIndex);
                const string& rightValue = rightTuple.at(rightIndex);
                // cout << "right name: " << rightName << " value: " << rightValue << endl;
            }
        }
        // Find one case where the attributes match but the values dont and return false
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