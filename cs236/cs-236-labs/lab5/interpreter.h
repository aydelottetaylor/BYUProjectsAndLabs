#pragma once
#include <vector>
#include <string>
#include "graph.h"
#include "parser.h"

using namespace std;

class Interpreter {
public:

    static Graph makeGraph(const vector<Rule>& rules) {
        int numRules = rules.size();
        Graph graph(numRules);

        for (int i = 0; i < numRules; ++i) {
            const Rule& currentRule = rules[i];
            const Predicate& headPredicate = currentRule.getHead();

            // cout << "from rule R" << i << ": " << headPredicate.getName() << "() :- ";

            const vector<Predicate>& bodyPredicates = currentRule.getBody();
            for (size_t j = 0; j < bodyPredicates.size(); ++j) {
                // cout << bodyPredicates[j].getName() << "()";
                // if (j != bodyPredicates.size() - 1)
                    // cout << ",";
            }
            // cout << endl;

            for (size_t j = 0; j < bodyPredicates.size(); ++j) {
                const string& currentBodyPredicateName = bodyPredicates[j].getName();
                // cout << "from body predicate: " << currentBodyPredicateName << "()" << endl;
                
                for (int l = 0; l < numRules; ++l) {
                    const Rule& currentToRule = rules[l];
                    const Predicate& headToPredicate = currentToRule.getHead();

                    // cout << "to rule R" << l << ": " << headToPredicate.getName() << "() :- ";

                    const vector<Predicate>& bodyToPredicates = currentToRule.getBody();
                    for (size_t k = 0; k < bodyToPredicates.size(); ++k) {
                        // cout << bodyToPredicates[k].getName() << "()";
                        // if (k != bodyToPredicates.size() - 1)
                            // cout << ",";
                    }
                    // cout << endl;

                    if (currentBodyPredicateName == headToPredicate.getName()) {
                        graph.addEdge(i, l);

                        // Print the dependency found
                        // cout << "dependency found: (R" << i << ",R" << l << ")" << endl;
                    }

                }
            }
        }
        return graph;
    }


};

// for (int i = 0; i < numRules; ++i) {
//     const Rule& currentRule = rules[i];

//     const Predicate& headPredicate = currentRule.getHead();

//     for (const auto& bodyPredicate : currentRule.getBody()) {
//         for (int j = 0; j < numRules; ++j) {
//             if (headPredicate.getName() == rules[j].getHead().getName()) {
//                 graph.addEdge(i, j);
//             }
//         }
//     }
// }