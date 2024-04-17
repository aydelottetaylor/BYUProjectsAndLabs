#pragma once
#include <iostream>
#include <set>

using namespace std;

class Node {
private:

public:
    set<int> adjacentNodeIDs;

    // Adds adjacentNode to node if there is an edge
    void addEdge(int adjacentNodeID) {
        adjacentNodeIDs.insert(adjacentNodeID);
    }

    // Returns true or false based on whether two nodes are adjacent
    bool hasEdgeTo(int nodeID) const {
        return adjacentNodeIDs.find(nodeID) != adjacentNodeIDs.end();
    }

    // Prints out node and adjacentNodes
    void toString() {
        auto it = adjacentNodeIDs.begin();
        if (it != adjacentNodeIDs.end()) {
            cout << "R" << *it;
            ++it;
            for (; it != adjacentNodeIDs.end(); ++it) {
                cout << ",R" << *it;
            }
        }
        cout << endl;
    }

};