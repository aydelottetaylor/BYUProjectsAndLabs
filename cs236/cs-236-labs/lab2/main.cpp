#include <stdio.h>
#include <iostream>
#include <fstream>
#include "token.h"
#include "parser.h"

using namespace std;

int main() {
    cout << "Id List portion (Part 2) without error" << endl;
    vector<Token> tokens = {
        Token(COMMA,",",2),
        Token(ID,"Ted",2),
        Token(COMMA,",",2),
        Token(ID,"Zed",2),
        Token(RIGHT_PAREN,")",2),
    };

    Parser p = Parser(tokens);
    p.idList();



    // cout << "Scheme portion (Part 3) with error" << endl;
    // vector<Token> tokens = {
    //     Token(ID,"Ned",2),
    //     Token(LEFT_PAREN,"(",2),
    //     Token(ID,"Ted",2),
    //     Token(COMMA,",",2),
    //     Token(ID,"Zed",2),
    //     Token(RIGHT_PAREN,")",2),
    // };

    // Parser p = Parser(tokens);
    // p.scheme();

    return 0;
}