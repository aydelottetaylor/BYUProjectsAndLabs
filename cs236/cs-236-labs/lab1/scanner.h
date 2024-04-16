#pragma once
#include <string>
#include "token.h"
#include <cctype>

using namespace std;

class Scanner {
private:
   string input;
   size_t currentPosition;
   size_t currentLine;
   size_t currentColumn;

public:
   Scanner(const string& input) : input(input) {}

   Token scanToken() {
      // Check for whitespace
      while(!input.empty() && isspace(input.at(0))) {
         input = input.substr(1);
      }

      // Check for commas
      if(!input.empty() && input.at(0) == ',') {
         TokenType type = COMMA;
         string value = ",";
         int line = 7; // Used 7 just as an example value

         // Remove the comma character from the input
         input = input.substr(1);

         return Token(type, value, line);
      }

      // Original code from stub function
      TokenType type = COMMA;
      string value = input;
      int line = 4;
      return Token(type, value, line);
   }


};
