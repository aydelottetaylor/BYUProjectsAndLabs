#pragma once 
#include "relation.h"
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

using namespace std;

class Database {

private:
    vector<Relation> relations;

public: 
    Database() {}

    void addRelation(const Relation& relation) {
        relations.push_back(relation);
    }

    Relation* getRelation(const string& name) {
        for (auto& rel : relations) {
            if(rel.getName() == name) {
                return &rel;
            }
        }
        return nullptr;
    }

    void printDatabase() {
        for(const auto& rel: relations) {
            cout << "Relation: " << rel.getName() << endl;
            cout << "Attributes: ";
            for (const auto& attr : rel.getAttributes()) {
                cout << attr << " ";
            }
            cout << endl;
            cout << "Tuples: " << endl;
            cout << rel.toString();
            cout << endl;
        }
    }

    void removeRelation(const string& name) {
        auto it = find_if(relations.begin(), relations.end(), [&](const Relation& relation) {
            return relation.getName() == name;
        });

        if (it != relations.end()) {
            relations.erase(it);
            cout << "Relation \"" << name << "\" removed from the database." << endl << endl;
        } else {
            cerr << "Error: Relation \"" << name << "\" not found in the database." << endl << endl;
        }
    }

};