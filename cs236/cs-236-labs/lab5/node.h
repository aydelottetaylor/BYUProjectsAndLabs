#pragma once
#include <iostream>
#include <set>

using namespace std;

class Node {
private:
    set<int> adjacentNodeIDs;

public:

    void addEdge(int adjacentNodeID) {
        adjacentNodeIDs.insert(adjacentNodeID);
    }

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