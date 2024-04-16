#pragma once
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

enum TokenType {
   COMMA,
   SEMICOLON,
   IDENTIFIER,
   INTEGER,
   FLOAT,
   KEYWORD_IF,
   KEYWORD_WHILE
};

class Token {
private:
   TokenType type;
   string value;
   int line;

public:
   Token(TokenType type, string value, int line) : type(type), value(value), line(line) {}

   string toString() const {
      stringstream out;
      out << "(" << typeName(type) << "," << "\"" << value << "\"" << "," << line << ")";
      return out.str();
   }

   string typeName(TokenType type) const {
      switch(type) {
         case COMMA: return "COMMA";
         case SEMICOLON: return "SEMICOLON";
         case IDENTIFIER: return "IDENTIFIER";
         case INTEGER: return "INTEGER";
         case FLOAT: return "FLOAT";
         case KEYWORD_IF: return "KEYWORD_IF";
         case KEYWORD_WHILE: return "KEYWORD_WHILE";
         default: return "UNKNOWN_TYPE";
      }
   }
};
