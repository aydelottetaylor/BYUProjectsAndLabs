#pragma once
#include <iostream>
#include <map>
#include "node.h"

using namespace std;

class Graph {
private:
    map<int,Node> nodes;

public:
    Graph(int size) {
        for (int nodeID = 0; nodeID < size; nodeID++) {
            nodes[nodeID] = Node();
        }
    }

    void addEdge(int fromNodeID, int toNodeID) {
        nodes[fromNodeID].addEdge(toNodeID);
    }

    void toString() {
        cout << "Dependency Graph" << endl;
        for (auto& pair : nodes) {
            int nodeID = pair.first;
            Node& node = pair.second;
            cout << "R" << nodeID << ":";
            node.toString();
        }
    }

};