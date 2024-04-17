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

    // Add relation to database
    void addRelation(const Relation& relation) {
        relations.push_back(relation);
    }

    // Takes a Relation name and returns a ponter to that relation
    Relation* getRelation(const string& name) {
        for (auto& rel : relations) {
            if(rel.getName() == name) {
                return &rel;
            }
        }
        return nullptr;
    }

    // Takes schemes and facts and uses them to create our databse object
    static Database createDatabase(vector<vector<string>> schemes, vector<vector<string>> facts) {
        Database db;

        for (auto& aScheme : schemes) {
            string name = aScheme.front();
            aScheme.erase(aScheme.begin());
            Scheme scheme(aScheme);
            Relation relation(name, scheme);
            db.addRelation(relation);
        }

        for (auto& tupleData : facts) {
            string relationName = tupleData.front();
            tupleData.erase(tupleData.begin());
            Relation* relation = db.getRelation(relationName);
            if(relation) {
                Tuple tuple(tupleData);
                relation->addTuple(tuple);
            } else {
                cerr << "Error: Relation \"" << relationName << "\" not found in Database." << endl;
            }
        }

        return db;
    }

    // Takes a database object and prints it out 
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
            cout << endl << endl;
        }
    }

    // Takes the name of a relation and removes that relation from the database
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