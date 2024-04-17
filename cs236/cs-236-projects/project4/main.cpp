#include "scheme.h"
#include "tuple.h"
#include "relation.h"
#include "database.h"
#include "interpreter.h"
#include "scanner.h"
#include "token.h"
#include "parser.h"
#include "parameter.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <algorithm>
#include <set>

using namespace std;

// Reads input from a given file and returns it as string input
string getInput(string inputFileName) {
   ifstream file(inputFileName);
   if(!file.is_open()) {
      cerr << "Error opening file: " << inputFileName << endl;
   }
   string input((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
   file.close();

   return input;
}

// Parses inputs and creates tokens, returns tokens as var vector<string> tokens
vector<Token> getTokens(string input) {
    Scanner scanner(input);
    Token token(TokenType::END_OF_FILE, "", 0);
    vector<Token> tokens = {};

    do {
        token = scanner.scanToken();
        if(token.getTokenType() != "NEW_LINE" && token.getTokenType() != "COMMENT") {
            tokens.push_back(token);
        }
    } while (token.getTokenType() != "END_OF_FILE");

    return tokens;
}

// Receives a value and an input vector, returns bool based on if value is in vector
bool notInVector(int value, vector<int>& vec) {
    return find(vec.begin(), vec.end(), value) == vec.end();
}

// Takes information from evaluating queries and returns what we need printed out
void printQueryName(string relationName, vector<string>query, Relation result, bool lastQuery, bool hasProjection = false) {
    int tupleCount = result.getTuples().size();
    
    cout << relationName << "(";
    for (size_t i = 1; i < query.size(); ++i) {
        cout << query[i];
        if (i < query.size() - 1) {
            cout << ",";
        }
    }
    cout << ")? ";
    if (tupleCount > 0) {
        cout << "Yes(" << tupleCount << ")";
        if(hasProjection) {
            cout << endl << result.toString();
        }
        if(!lastQuery) {
            cout << endl;
        }
    } else {
        cout << "No";
        if(!lastQuery) {
            cout << endl;
        }
    }
}

// Receives a query that does not require a projection operation and evaluates
void evaluateNonProjection(vector<string> query, Database db, bool lastQuery) {
    string relationName = query[0];
    string attributeOne = query[1];
    Relation* relationPtr = db.getRelation(relationName);
    Relation result;

    result = relationPtr->select(0, attributeOne);
    for (size_t i = 1; i < query.size(); ++i) {
        string attribute = query[i];
        if(attribute[0] == '\'') {
            result = result.select(i-1, attribute);
        }
    }

    printQueryName(relationName, query, result, lastQuery);
}

// Receives a query that requires a projection but does not select anything and evaluates
void evaluateNoSelectProjection(vector<string> query, Database db, bool lastQuery) {
    string relationName = query[0];
    Relation* relationPtr = db.getRelation(relationName);
    Scheme scheme = relationPtr->getAttributes();
    Relation result;

    result = relationPtr->rename(scheme);

    set<string> queryStrings;
    for (size_t i = 1; i < query.size(); ++i) {
        string queryString = query[i];
        queryStrings.insert(queryString);
        string schemeString = scheme[i - 1];
        if(queryString != schemeString) {
            result = result.renameSingleAttribute(queryString, i - 1);
        }
    }

    vector<string> myVector(queryStrings.begin(), queryStrings.end());

    for(string& attr: myVector) {
        set<int> instances;
        for(size_t i = 1; i < query.size(); ++i) {
            string speAttribute = query[i];
            if(attr[0] == speAttribute[0]) {
                instances.insert(i-1);
            }
        }
        result = result.selectTuplesWithSame(instances);
    }

    vector<string> attributesToProject;
    set<string> seenAttributes;
    for (size_t i = 1; i < query.size(); ++i) {
        string attribute = query[i];
        if(attribute[0] != '\'') {
            if (seenAttributes.find(attribute) == seenAttributes.end()) {
                attributesToProject.push_back(attribute); // Add to the vector
                seenAttributes.insert(attribute);    // Add to the set to mark it as seen
            }
        }
    }

    if(attributesToProject.size() > 0) {
        result = result.project({attributesToProject});
    }
    
    printQueryName(relationName, query, result, lastQuery, true);
}

// Receives a query that needs both a selection and a projection and evaluates
void evaluateSelectProjectionQuery(vector<string> query, Database db, bool lastQuery) {
    string relationName = query[0];
    Relation* relationPtr = db.getRelation(relationName);
    Scheme scheme = relationPtr->getAttributes();
    Relation result;

    vector<int> positionOfSorts;
    for(size_t i = 1; i < query.size(); ++i) {
        string attribute = query[i];
        if(attribute[0] == '\'') {
            positionOfSorts.push_back(i);
        }
    }

    set<string> queryStrings;
    for (size_t i = 1; i < query.size(); ++i) {
        string attr = query[i];
        if(attr[0] != '\'') {
            queryStrings.insert(attr);
        }
    }

    for (size_t i = 0; i < positionOfSorts.size(); ++i) {
        int attributePositionToSort = positionOfSorts[i] - 1;
        int pos = positionOfSorts[i];
        string attributeToSort = query[positionOfSorts[i]];
        if (i == 0) {
            result = relationPtr->select(attributePositionToSort, attributeToSort);
        } else {
            result = result.select(attributePositionToSort, attributeToSort);
        }
    }

    vector<string> myVector(queryStrings.begin(), queryStrings.end());
    for(string& attr: myVector) {
        set<int> instances;
        for(size_t i = 1; i < query.size(); ++i) {
            string speAttribute = query[i];
            if(attr == speAttribute) {
                instances.insert(i-1);
            }
        }
        result = result.selectTuplesWithSame(instances);
    }

    for (size_t i = 1; i < query.size(); ++i) {
        string queryString = query[i];
        string schemeString = scheme[i - 1];
        if(queryString != schemeString) {
            if(notInVector(i, positionOfSorts)) {
                result = result.renameSingleAttribute(queryString, i - 1);

            }
        }
    }

    vector<string> projectionList;
    for(size_t i = 1; i < query.size(); ++i) {
        if(notInVector(i, positionOfSorts)) {
            string columnToProject = query[i];
            projectionList.push_back(columnToProject);
        }
    }

    vector<string> attributesToProject;
    set<string> seenAttributes;
    for (size_t i = 1; i < query.size(); ++i) {
        string attribute = query[i];
        if(attribute[0] != '\'') {
            if (seenAttributes.find(attribute) == seenAttributes.end()) {
                attributesToProject.push_back(attribute);
                seenAttributes.insert(attribute);
            }
        }
    }

    if(attributesToProject.size() > 0) {
        result = result.project({attributesToProject});
    }

    printQueryName(relationName, query, result, lastQuery, true);
}

// Receives query and returns bool based on whether it requires a projection operation or not
bool evaluateIfReqProjection(vector<string> query) {
    for (size_t i = 1; i < query.size(); ++i) {
        string attribute = query[i];
        if(attribute[0] != '\'') {
            return true;
        }
    }
    return false;
}

// Reveives a query and returns bool based on whether it requires a select or not
bool evaluateIfNoSelect(vector<string> query) {
    for (size_t i = 1; i < query.size(); ++i) {
        string attribute = query[i];
        if(attribute[0] == '\'') {
            return true;
        }
    }
    return false;
}

// Takes a vector<vector<string>> and passes each vector<string> query into other functions that evaluate it
void evaluateQueries(Database db, const vector<vector<string>>& queries) {
    cout << "Query Evaluation" << endl;
    int i = 0;
    for (const auto& query : queries) {
        ++i;
        bool lastQuery = false;
        if(i == queries.size()) {
            lastQuery = true;
        }
        string relationName = query[0];
        bool isprojection = evaluateIfReqProjection(query);

        if(!isprojection) {
            evaluateNonProjection(query, db, lastQuery);
        } else if (isprojection) {
            bool noSelect = evaluateIfNoSelect(query);
            
            if(!noSelect) {
                evaluateNoSelectProjection(query, db, lastQuery);
            } else {
                evaluateSelectProjectionQuery(query, db, lastQuery);
            }
        }
    }
}

vector<string> getAttributes(string relationString) {
    vector<string> attributes;
    size_t startPos = relationString.find('(') + 1;
    size_t endPos = relationString.find(')');
    string attrStr = relationString.substr(startPos, endPos - startPos);
    size_t pos = 0;
    while ((startPos = attrStr.find(',', pos)) != string::npos) {
        attributes.push_back(attrStr.substr(pos, startPos - pos));
        pos = startPos + 1;
    }
    attributes.push_back(attrStr.substr(pos));

    return attributes;
}

Relation performJoin (vector<Relation> intermediateRelations) {
    Relation joinResult;
    joinResult = intermediateRelations[0];
    for(int i = 1; i < intermediateRelations.size(); i++) {
        joinResult = joinResult.join(intermediateRelations[i]);
    }
    return joinResult;
}

void evaluateRules(Database& db, vector<Rule>& rules) {
    cout << "Rule Evaluation" << endl;
    bool addedTuple = true;
    int passes = 0;

    do {
        addedTuple = false;
        passes++;
        for(auto& rule : rules) {
            cout << rule.toString() << endl;
            
            Relation result;
            vector<Relation> intermediateRelations;

            for(auto& predicate : rule.bodyPredicates) {
                string relationString = predicate.toString();
                string relationName = relationString.substr(0, relationString.find('('));
                Relation* relationPtr = db.getRelation(relationName);
                Scheme scheme = relationPtr->getAttributes();
                Relation result;

                result = relationPtr->rename(scheme);

                vector<string> attributes = getAttributes(relationString);

                for (size_t i = 0; i < attributes.size(); ++i) {
                    string ruleAttribute = attributes[i];
                    string schemeAttribute = scheme[i];
                    if(ruleAttribute != schemeAttribute) {
                        result = result.renameSingleAttribute(ruleAttribute, i);
                    }
                }

                vector<int> positionOfSorts;
                for(size_t i = 0; i < attributes.size(); ++i) {
                    if(attributes[i].find('\'') != string::npos) {
                        // cout << attributes[i] << " position: " << i << endl;
                        positionOfSorts.push_back(i);
                    }
                }

                set<string> queryStrings;
                for (size_t i = 0; i < attributes.size(); ++i) {
                    if(attributes[i].find('\'') == string::npos) {
                        queryStrings.insert(attributes[i]);
                    }
                }

                vector<string> myVector(queryStrings.begin(), queryStrings.end());
                for(string& attr: myVector) {
                    set<int> instances;
                    for(size_t i = 0; i < attributes.size(); ++i) {
                        string speAttribute = attributes[i];
                        if(attr == speAttribute) {
                            instances.insert(i);
                        }
                    }
                    result = result.selectTuplesWithSame(instances);
                }

                if(positionOfSorts.size() > 0) {
                    for (size_t i = 0; i < positionOfSorts.size(); ++i) {
                        int attributePositionToSort = positionOfSorts[i];
                        string attributeToSort = attributes[positionOfSorts[i]];
                        result = result.select(attributePositionToSort, attributeToSort);
                    }
                }

                intermediateRelations.push_back(result);
            }

            result = performJoin(intermediateRelations);
            
            string headPredicateString = rule.headPredicate.toString();
            string relationName = headPredicateString.substr(0, headPredicateString.find('('));
            vector<string> attributes = getAttributes(headPredicateString);
        
            if(attributes.size() > 0) {
                result = result.project({attributes});
            }

            Relation* newRelation = db.getRelation(relationName);
            Scheme newRelationScheme = newRelation->getAttributes();

            for(int i = 0; i < newRelationScheme.size(); i++) {
                result = result.renameSingleAttribute(newRelationScheme[i], i);
            }

            int beforeUnionSize = newRelation->getTuples().size();
            newRelation->Union(result);

            if(newRelation->getTuples().size() > beforeUnionSize) {
                addedTuple = true;
            }
        }
    } while (addedTuple);
    cout << endl << "Schemes populated after " << passes << " passes through the Rules." << endl << endl;
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        cerr << "Make sure that you include the name of the file to scan. Ex. ./main docToScan.txt" << endl;
        return 1;
    }

    string inputFileName = argv[1];
    string input = getInput(inputFileName);

    vector<Token> tokens = getTokens(input);

    Parser p = Parser(tokens);
    DatalogProgram datalogProgram;
    p.runDatalogProgram();

    vector<vector<string>> schemes = p.getTheSchemes();
    vector<vector<string>> facts = p.getTheFacts();
    vector<Rule> rules = p.getTheRules();
    vector<vector<string>> queries = p.getTheQueries();

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

    evaluateRules(db, rules);

    // db.printDatabase();

    evaluateQueries(db, queries);
    
}