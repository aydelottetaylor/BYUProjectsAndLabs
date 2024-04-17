#pragma once
#include <vector>
#include <string>
#include <stack>
#include <algorithm>
#include "graph.h"
#include "scc.h"

using namespace std;

// In interpreter.h we perform the following:
//      - Rule Optimization by:
//          - DFS on Graphs and Reverse Graphs
//          - Finding SCCs
//      - Evaluate Rules
//      - Evaluate Queries
// 
// Could honestly probably refactor and seperate out better, will look into if I can find the time haha
// 
// Taylor Aydelotte 4/10/2024

class Interpreter {
public:

    // Performs a regular join on on two intermediate relations
    static Relation performJoin (vector<Relation> intermediateRelations) {
        Relation joinResult;
        joinResult = intermediateRelations[0];
        for(int i = 1; i < intermediateRelations.size(); i++) {
            joinResult = joinResult.join(intermediateRelations[i]);
        }
        return joinResult;
    }

    // Retrieves attributes from given relation string
    static vector<string> getAttributes(string relationString) {
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

    // Takes a Database object, vector of rules, and bool of whether to only evaluate that 
    // (usually one rule) rule once or not, evaluates those rules using db
    static void evaluateRules(Database& db, vector<Rule>& rules, bool evaluateOnce) {
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
        } while (addedTuple && !evaluateOnce);
        cout << passes << " passes: ";
    }

    // Takes a vector<vector<string>> and passes each vector<string> query into other functions that evaluate it
    static void evaluateQueries(Database db, const vector<vector<string>>& queries) {
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
        cout << endl;
    }

    // Receives query and returns bool based on whether it requires a projection operation or not
    static bool evaluateIfReqProjection(vector<string> query) {
        for (size_t i = 1; i < query.size(); ++i) {
            string attribute = query[i];
            if(attribute[0] != '\'') {
                return true;
            }
        }
        return false;
    }

    // Reveives a query and returns bool based on whether it requires a select or not
    static bool evaluateIfNoSelect(vector<string> query) {
        for (size_t i = 1; i < query.size(); ++i) {
            string attribute = query[i];
            if(attribute[0] == '\'') {
                return true;
            }
        }
        return false;
    }

    // Receives a value and an input vector, returns bool based on if value is in vector
    static bool notInVector(int value, vector<int>& vec) {
        return find(vec.begin(), vec.end(), value) == vec.end();
    }

    // Receives a query that needs both a selection and a projection and evaluates
    static void evaluateSelectProjectionQuery(vector<string> query, Database db, bool lastQuery) {
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

    // Receives a query that requires a projection but does not select anything and evaluates
    static void evaluateNoSelectProjection(vector<string> query, Database db, bool lastQuery) {
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

    // Receives a query that does not require a projection operation and evaluates
    static void evaluateNonProjection(vector<string> query, Database db, bool lastQuery) {
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

    // Takes information from evaluating queries and returns what we need printed out
    static void printQueryName(string relationName, vector<string>query, Relation result, bool lastQuery, bool hasProjection = false) {
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

    // Performs pre-evaluation steps on given rules and SCCs and then calls evaluate rules based on SCCs 
    static void preEvalRules(Database& db, vector<Rule>& rules, vector<SCC> sccs) {
        for(const SCC& scc : sccs) {
            if(scc.otherNodes.size() != 0) {
                set<int> nodes(scc.otherNodes.begin(), scc.otherNodes.end());
                cout << "SCC: R";
                auto it = nodes.begin();
                while (it != nodes.end()) {
                    if (it != nodes.begin()) {
                        cout << ",R" << *it;
                    } else {
                        cout << *it;
                    }
                    ++it;
                }
                cout << endl;
                vector<Rule> theRules;
                
                for(auto& node : nodes) {
                    theRules.push_back(rules[node]);
                }
                evaluateRules(db, theRules, false);

                it = nodes.begin();
                cout << "R";
                while (it != nodes.end()) {
                    if (it != nodes.begin()) {
                        cout << ",R" << *it;
                    } else {
                        cout << *it;
                    }
                    ++it;
                }
                cout << endl;

            } else {
                scc.print();
                vector<Rule> theRule;
                theRule.push_back(rules[scc.firstNode]);
                evaluateRules(db, theRule, true);
                cout << "R" << scc.firstNode << endl;
            }
        }
        cout << endl;
    }

    // Takes rules and creates a graph
    static Graph makeGraph(const vector<Rule>& rules) {
        int numRules = rules.size();
        Graph graph(numRules);

        for (int i = 0; i < numRules; ++i) {
            const Rule& currentRule = rules[i];
            const Predicate& headPredicate = currentRule.getHead();

            const vector<Predicate>& bodyPredicates = currentRule.getBody();

            for (size_t j = 0; j < bodyPredicates.size(); ++j) {
                const string& currentBodyPredicateName = bodyPredicates[j].getName();
                
                for (int l = 0; l < numRules; ++l) {
                    const Rule& currentToRule = rules[l];
                    const Predicate& headToPredicate = currentToRule.getHead();

                    if (currentBodyPredicateName == headToPredicate.getName()) {
                        graph.addEdge(i, l);
                    }

                }
            }
        }
        return graph;
    }

    // Takes rules and makes a reverse edge graph
    static Graph makeReverseGraph(const vector<Rule>& rules) {
        int numRules = rules.size();
        Graph graph(numRules);

        for (int i = 0; i < numRules; ++i) {
            const Rule& currentRule = rules[i];
            const Predicate& headPredicate = currentRule.getHead();

            const vector<Predicate>& bodyPredicates = currentRule.getBody();

            for (size_t j = 0; j < bodyPredicates.size(); ++j) {
                const string& currentBodyPredicateName = bodyPredicates[j].getName();

                for (int l = 0; l < numRules; ++l) {
                    const Rule& currentToRule = rules[l];
                    const Predicate& headToPredicate = currentToRule.getHead();

                    const vector<Predicate>& bodyToPredicates = currentToRule.getBody();

                    for (size_t k = 0; k < bodyToPredicates.size(); ++k) {
                        if (currentBodyPredicateName == headToPredicate.getName()) {
                            graph.addEdge(l, i); // Reverse the edge direction
                        }
                    }
                }
            }
        }
        return graph;
    }

    // Runs a DFS on a given graph, in this case we are using it on the reverse graph to optimize rule evaluation
    static stack<int> runDFSForest(const Graph& reverseGraph) {
        int numNodes = reverseGraph.size();
        vector<bool> visited(numNodes, false);
        stack<int> orderedNodesStack; // Stack to store the ordered nodeIDs

        for (int i = 0; i < numNodes; ++i) {
            if (!visited[i]) {
                DFS(reverseGraph, i, visited, orderedNodesStack); // Pass stack instead of postorder vector
            }
        }

        return orderedNodesStack;
    }

    // Takes a graph and a stack of ordered nodes and uses that to find Strongly Connected Components
    static vector<SCC> findSCCs(const Graph& graph, stack<int>& orderedNodesStack) {
        int numNodes = graph.size();
        vector<bool> visited(numNodes, false);
        vector<SCC> sccs;

        while (!orderedNodesStack.empty()) {
            int startNode = orderedNodesStack.top();
            orderedNodesStack.pop();
            vector<bool> startNodes(numNodes, false);

            if (!visited[startNode]) {
                startNodes[startNode] = true;
                SCC scc(startNode);
                DFSSCC(graph, startNode, visited, scc, startNodes, startNode);
                sccs.push_back(scc);
            }
        }

        return sccs;
    }

private:

    // Helper function for findSCCs()
    static void DFSSCC(const Graph& graph, int nodeID, vector<bool>& visited, SCC& scc, vector<bool>& startNodes, int startNode) {
        visited[nodeID] = true;
        const Node& currentNode = graph.nodes.at(nodeID);
        const set<int>& adjacentNodeIDs = currentNode.adjacentNodeIDs;

        if (!adjacentNodeIDs.empty()) {
            for (int neighbor : adjacentNodeIDs) {
                if (!visited[neighbor]) {
                    DFSSCC(graph, neighbor, visited, scc, startNodes, startNode);
                } else if (neighbor == startNode) {
                    scc.addNode(neighbor);
                }
            }
        }

        if(!startNodes[nodeID]) {
            scc.addNode(nodeID);
        } else if (nodeID == startNode && !adjacentNodeIDs.empty() && !startNodes[nodeID]) {
            scc.addNode(nodeID);
        } 
    }

    // Helper function for runDFSForest(), helps perform DFS on given graph
    static void DFS(const Graph& graph, int nodeID, vector<bool>& visited, stack<int>& orderedNodesStack) {
        visited[nodeID] = true;
        
        const Node& currentNode = graph.nodes.at(nodeID);
        const set<int>& adjacentNodeIDs = currentNode.adjacentNodeIDs;

        for (int neighbor : adjacentNodeIDs) {
            if (!visited[neighbor]) {
                DFS(graph, neighbor, visited, orderedNodesStack);
            }
        }

        orderedNodesStack.push(nodeID);
    }

};