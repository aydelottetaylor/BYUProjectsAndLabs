#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <iostream>

using namespace std;

class Tuple : public vector<string> {

public:
    Tuple() = default;

    Tuple(vector<string> values) : vector<string>(values) { }

    // Print out a tuple
    string toString(const Scheme& scheme) const {
        stringstream out;
        out << "  ";
        for (size_t i = 0; i < scheme.size(); ++i) {
            out << scheme.at(i) << "=" << this->getValueByAttribute(scheme.at(i), scheme);
            if (i != scheme.size() - 1) {
                out << ", ";
            }
        }
        return out.str();
    }

    // Gets the data from a tuple given its scheme
    map<string, string> getData(const Scheme& scheme) const {
        map<string, string> data;
        for (size_t i = 0; i < scheme.size(); ++i) {
            data[scheme.at(i)] = this->at(i);
        }
        return data;
    }

    void addAttribute(const string& value) {
        this->push_back(value);
    }

    // Set the attributes to be projected given an original tuple, the scheme, and the attributes you want to project
    void setProjectedAttributes(const vector<string>& projectedAttributes, const Tuple& originalTuple, const Scheme& scheme) {
        this->clear();
        for (const auto& attribute : projectedAttributes) {
            this->push_back(originalTuple.getValueByAttribute(attribute, scheme));
        }
    }

    // Gets the value of a tuple based on a given attribute name and a given scheme
    string getValueByAttribute(const string& attribute, const Scheme& scheme) const {
        for (size_t i = 0; i < scheme.size(); ++i) {
            if (scheme[i] == attribute) {
                if (i < this->size()) {
                    return this->at(i);
                }
                break;
            }
        }
        return "";
    }

};