#include <vector>
#include <stack>

class SCC {
public:    
    int firstNode;
    vector<int> otherNodes;

    SCC(int firstNode) : firstNode(firstNode) {}

    void addNode(int node) {
        otherNodes.push_back(node);
    }

    void print() const {
        cout << "SCC: R" << firstNode;
        for (int node : otherNodes) {
            cout << ", R" << node;
        }
        cout << endl;
    }

};