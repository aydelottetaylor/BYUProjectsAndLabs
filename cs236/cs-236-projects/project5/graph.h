#pragma once
#include <iostream>
#include <map>
#include "node.h"

using namespace std;

class Graph {
private:
    

public:
    map<int,Node> nodes;
    
    Graph(int size) {
        for (int nodeID = 0; nodeID < size; nodeID++) {
            nodes[nodeID] = Node();
        }
    }

    // Returns size of given graph
    int size() const {
        return nodes.size();
    }

    // Add an edge to graph using given fromNodeID and toNodeID
    void addEdge(int fromNodeID, int toNodeID) {
        nodes[fromNodeID].addEdge(toNodeID);
    }

    // Prints out dependency graph given graph
    void toString() {
        cout << "Dependency Graph" << endl;
        for (auto& pair : nodes) {
            int nodeID = pair.first;
            Node& node = pair.second;
            cout << "R" << nodeID << ":";
            node.toString();
        }
        cout << endl;
    }

    // Prints out reverse dependency graph given graph
    void toStringReverse() {
        cout << "Reverse Dependency Graph" << endl;
        for (auto& pair : nodes) {
            int nodeID = pair.first;
            Node& node = pair.second;
            cout << "R" << nodeID << ":";
            node.toString();
        }
        cout << endl;
    }

};