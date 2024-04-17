#include "scheme.h"
#include "tuple.h"
#include "relation.h"
#include "database.h"
#include "scanner.h"
#include "token.h"
#include "parser.h"
#include "graph.h"
#include "interpreter.h"
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

int main(int argc, char *argv[]) {
    if(argc != 2) {
        cerr << "Make sure that you include the name of the file to scan. Ex. ./main docToScan.txt" << endl;
        return 1;
    }

    string inputFileName = argv[1];
    string input = getInput(inputFileName);

    vector<Token> tokens = getTokens(input);

    Parser p = Parser(tokens);
    p.runDatalogProgram();

    vector<vector<string>> schemes = p.getTheSchemes();
    vector<vector<string>> facts = p.getTheFacts();
    vector<Rule> rules = p.getTheRules();
    vector<vector<string>> queries = p.getTheQueries();

    Database db = Database::createDatabase(schemes, facts);
    
    Graph graph = Interpreter::makeGraph(rules);
    graph.toString();
    Graph reverseGraph = Interpreter::makeReverseGraph(rules);
    stack<int> orderedNodesStack = Interpreter::runDFSForest(reverseGraph);

    vector<SCC> sccs = Interpreter::findSCCs(graph, orderedNodesStack);

    cout << "Rule Evaluation" << endl;
    Interpreter::preEvalRules(db, rules, sccs);

    // db.printDatabase();

    Interpreter::evaluateQueries(db, queries);
    
}