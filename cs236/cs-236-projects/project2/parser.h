#include <iostream>
#include <vector>
#include <set>
#include "token.h"
#include "parameter.h"

using namespace std;

class Parameter;

class Predicate {
public:
    string name;
    vector<Parameter> parameters;

    string toString() const {
        string result = name + "(";
        for (size_t i = 0; i < parameters.size(); ++i) {
            result += parameters[i].toString();
            if (i != parameters.size() - 1) {
                result += ",";
            }
        }
        result += ")";
        return result;
    }
};

class Rule {
public:
    Predicate headPredicate;
    vector<Predicate> bodyPredicates;

    string toString() const {
        string result = headPredicate.toString() + " :- ";
        for (size_t i = 0; i < bodyPredicates.size(); ++i) {
            result += bodyPredicates[i].toString();
            if (i != bodyPredicates.size() - 1) {
                result += ",";
            }
        }
        result += ".";
        return result;
    }
};

class DatalogProgram {
public:
    vector<Predicate> schemes;
    vector<Predicate> facts;
    vector<Rule> rules;
    vector<Predicate> queries;
    set<string> domain;

    string toString() const {
        string result = "Schemes(" + to_string(schemes.size()) + "):\n";
        for (const auto& scheme : schemes) {
            result += "  " + scheme.toString() + "\n";
        }
        result += "Facts(" + to_string(facts.size()) + "):\n";
        for (const auto& fact : facts) {
            result += "  " + fact.toString() + ".\n";
        }
        result += "Rules(" + to_string(rules.size()) + "):\n";
        for (const auto& rule : rules) {
            result += "  " + rule.toString() + "\n";
        }
        result += "Queries(" + to_string(queries.size()) + "):\n";
        for (const auto& query : queries) {
            result += "  " + query.toString() + "?\n";
        }
        result += "Domain(" + to_string(domain.size()) + "):\n";
        int i = 0;
        for (const auto& value : domain) {
            result += "  " + value + "\n";
        }
        return result;
    }
};

class Parser {
private:
    vector<Token> tokens;
    DatalogProgram datalogProgram;

public:
    Parser(const vector<Token>& tokens) : tokens(tokens) { }

    TokenType tokenType() const {
        return tokens.at(0).getType();
    }

    void advanceToken() {
        if (tokens.size() > 0) {
            tokens.erase(tokens.begin());
        }
    }

    string tokenToString() const {
        if (!tokens.empty()) {
            return tokens.front().getStringValue();
        }
        return "";
    }

    void throwError() {
        cout << "  ";
        if(tokenType() != 18) {
            cout << tokens.at(0).toString() << endl;
        } else { 
            cout << "(EOF,\"\",17)" << endl;
        }
    }

    void match(TokenType t) {
        if (tokenType() == t) {
            advanceToken();
        } else {
            cout << "Failure!" << endl;
            throwError();
            exit(EXIT_SUCCESS);
        }
    }

    // ------ DATALOG GRAMMAR ------ //

    void runDatalogProgram() {
        getSchemes();
        getFacts();
        getRules();
        getQueries();
        match(END_OF_FILE);
        cout << "Success!" << endl;
        printDatalogProgram();
    }

    void getSchemes() {
        match(SCHEMES);
        match(COLON);
        if (tokenType() != ID) {
            cout << "Failure!" << endl;
            throwError();
            exit(EXIT_SUCCESS);
        }
        while (tokenType() == ID) {
            datalogProgram.schemes.push_back(scheme());
        }
    }

    void getFacts() {
        match(FACTS);
        match(COLON);
        while (tokenType() == ID) {
            datalogProgram.facts.push_back(fact());
        }
    }

    void getRules() {
        match(RULES);
        match(COLON);
        while (tokenType() == ID) {
            datalogProgram.rules.push_back(rule());
        }
    }

    void getQueries() {
        match(QUERIES);
        match(COLON);
        if (tokenType() != ID) {
            cout << "Failure!" << endl;
            throwError();
            exit(EXIT_SUCCESS);
        }
        while (tokenType() == ID) {
            datalogProgram.queries.push_back(query());
        }
    }

    Predicate scheme() {
        Predicate pred;
        pred.name = tokenToString();
        match(ID);
        match(LEFT_PAREN);
        pred.parameters.push_back(parameter());
        idList(pred.parameters, 1);
        match(RIGHT_PAREN);
        return pred;
    }

    Predicate fact() {
        Predicate pred;
        pred.name = tokenToString();
        match(ID);
        match(LEFT_PAREN);
        if (tokenType() != RIGHT_PAREN) {
            
            string value = tokenToString();
            datalogProgram.domain.insert(value);
            pred.parameters.push_back(parameter());
            stringList(pred.parameters);
        } else {
            cout << "Failure!" << endl;
            throwError();
            exit(EXIT_SUCCESS);
        }
        match(RIGHT_PAREN);
        match(PERIOD);
        return pred;
    }

    Rule rule() {
        Rule r;
        r.headPredicate = headPredicate();
        match(COLON_DASH);
        r.bodyPredicates.push_back(predicate());
        predicateList(r.bodyPredicates);
        match(PERIOD);
        return r;
    }

    Predicate query() {
        Predicate pred;
        pred = predicate();
        match(Q_MARK);
        return pred;
    }

    Predicate headPredicate() {
        Predicate pred;
        pred.name = tokenToString();
        match(ID);
        match(LEFT_PAREN);
        pred.parameters.push_back(parameter());
        idList(pred.parameters);
        match(RIGHT_PAREN);
        return pred;
    }

    Predicate predicate() {
        Predicate pred;
        pred.name = tokenToString();
        match(ID);
        match(LEFT_PAREN);
        pred.parameters.push_back(parameter());
        parameterList(pred.parameters);
        match(RIGHT_PAREN);
        return pred;
    }

    void predicateList(vector<Predicate>& preds) {
        if (!tokens.empty() && tokenType() == COMMA) {
            match(COMMA);
            preds.push_back(predicate());
            predicateList(preds);
        }
    }

    void parameterList(vector<Parameter>& params) {
        if (!tokens.empty() && tokenType() == COMMA) {
            match(COMMA);
            params.push_back(parameter());
            parameterList(params);
        }
    }

    void stringList(vector<Parameter>& params) {
        if (!tokens.empty() && tokenType() == COMMA) {
            match(COMMA);
            Parameter param;
            param.value = tokenToString();
            match(STRING);
            params.push_back(param);
            datalogProgram.domain.insert(param.value);
            stringList(params);
        }
    }

    void idList(vector<Parameter>& params, int isScheme = 0) {
        if (!tokens.empty() && tokenType() == COMMA) {
            match(COMMA);
            if(isScheme == 1 && tokenType() != ID) {
                cout << "Failure!" << endl;
                throwError();
                exit(EXIT_SUCCESS);
            }
            params.push_back(parameter());
            idList(params);
        }
    }

    Parameter parameter() {
        Parameter param;
        if (tokenType() == STRING || tokenType() == ID) {
            param.value = tokenToString();
            advanceToken();
        } else {
            cout << "Failure!" << endl;
            throwError();
            exit(EXIT_SUCCESS);
        }
        return param;
    }

    void printDatalogProgram() {
        cout << datalogProgram.toString();
    }
};
