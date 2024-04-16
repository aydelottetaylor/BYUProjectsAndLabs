#include <vector>
#include "interpreter.h"
#include "node.h"
#include "graph.h"
#include "rule.h"

using namespace std;

int main() {
    pair<string,vector<string>> ruleNames[] = {
        { "A", { "B", "C" } },
        { "B", { "A", "D" } },
        { "B", { "B" } },
        { "E", { "F", "G" } },
        { "E", { "E", "F" } },
    };

    vector<Rule> rules;

    for (auto& rulePair : ruleNames) {
        std::string headName = rulePair.first;
        Rule rule = Rule(Predicate(headName));
        std::vector<std::string> bodyNames = rulePair.second;
        for (auto& bodyName : bodyNames)
            rule.addBodyPredicate(Predicate(bodyName));
        rules.push_back(rule);
    }

    Graph graph = Interpreter::makeGraph(rules);
    graph.toString();

    return 0;
}