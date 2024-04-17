#pragma once
#include <string>
#include "token.h"
#include <cctype>
#include <cstdio>

using namespace std;


// Reeeaaaaly need to refactor scanToken() lol, will do so if I find some time to do it
// 
// 
// 

class Scanner {
private:
   string input;
   size_t currentPosition = 0;
   size_t currentLine = 1;
   size_t tokenNumber = 0;

public:
   Scanner(const string& input) : input(input), currentPosition(0) {}

   // Grab a character in the input at the given position
   char getCurrentChar(int lookAtPosition) {
      if(currentPosition <= input.length()) {
         return input[lookAtPosition];
      }
      return '\0';
   }

   // Grabs a possible string for "Schemes", "Facts", "Rules" or "Queries"
   // based on the number that is passed through the addition var
   string checkForString(int currentPosition, int addition) {
      int newPosition = currentPosition + addition;
      if(static_cast<size_t>(newPosition) <= input.length()) {
         return input.substr(currentPosition, addition);
      }
      return "";
   }

   // Skip over unimportant/unneccessary white space
   void skipWhiteSpace() {
      while(isspace(getCurrentChar(currentPosition)) && (getCurrentChar(currentPosition) != '\n')) {
         currentPosition++;
      }
   }

   // Scanning logic (can probably refactor this to make it more manageable)
   Token scanToken() {
      skipWhiteSpace();

      char currentChar = getCurrentChar(currentPosition);

      TokenType type;
      string value;

      if(currentChar == '\0') { // Checks for END_OF_FILE
         type = END_OF_FILE;
         value = "";

         return Token(type, value, currentLine);
      } else if(currentChar == ',') { // Checks for COMMA
         type = COMMA;
         value = currentChar;

         currentPosition++;

         return Token(type, value, currentLine);
      } else if(currentChar == '.') { // Checks for PERIOD
         type = PERIOD;
         value = currentChar;

         currentPosition++;
         return Token(type, value, currentLine);
      } else if(currentChar == '?') { // Checks for Q_MARK
         type = Q_MARK;
         value = currentChar;

         currentPosition++;
         return Token(type, value, currentLine);
      } else if(currentChar == '(') { // Checks for LEFT_PAREN
         type = LEFT_PAREN;
         value = currentChar;

         currentPosition++;
         return Token(type, value, currentLine);
      } else if(currentChar == ')') { // Checks for RIGHT_PAREN
         type = RIGHT_PAREN;
         value = currentChar;

         currentPosition++;
         return Token(type, value, currentLine);
      } else if(currentChar == '\n') { // Checks for NEW_LINE char
         currentLine++;

         type = NEW_LINE;
         value = "";

         currentPosition++;
         return Token(type, value, currentLine);
      } else if(currentChar == ':') { // Checks for COLON char
         char nextChar = getCurrentChar(currentPosition + 1);
         if(nextChar == '-') { // If next char is DASH then type is COLON_DASH
            type = COLON_DASH;
            value = ":-";

            currentPosition = currentPosition + 2;
            return Token(type, value, currentLine);
         } else {
            type = COLON;
            value = currentChar;

            currentPosition++;
            return Token(type, value, currentLine);
         }
      } else if(currentChar == '*') { // Checks for MULTIPLY char
         type = MULTIPLY;
         value = currentChar;

         currentPosition++;
         return Token(type, value, currentLine);
      } else if(currentChar == '+') { // Checks for ADD char
         type = ADD;
         value = currentChar;

         currentPosition++;
         return Token(type, value, currentLine);
      } else if(currentChar == 'S' || currentChar == 'F' || currentChar == 'R' || currentChar == 'Q') { // Checks for chars 'S', 'F', 'R', 'Q'
         string possibleString = checkForString(currentPosition, 7);

         if(possibleString == "Schemes" && !isalnum(getCurrentChar(currentPosition + 7))) { // Grabs possible string and checks to see if it matches "Schemes"
            type = SCHEMES;
            value = possibleString;

            currentPosition = currentPosition + 7;
            return Token(type, value, currentLine);
         } else {
            string possibleString = checkForString(currentPosition, 5);
            if(possibleString == "Facts" && !isalnum(getCurrentChar(currentPosition + 5))) { // Grabs possible string and checks to see if it matches "Facts"
               type = FACTS;
               value = possibleString;

               currentPosition = currentPosition + 5;
               return Token(type, value, currentLine);
            } else {
               string possibleString = checkForString(currentPosition, 5);

               if(possibleString == "Rules" && !isalnum(getCurrentChar(currentPosition + 5))) { // Grabs possible string and checks to see if it matches "Rules"
               type = RULES;
               value = possibleString;

               currentPosition = currentPosition + 5;
               return Token(type, value, currentLine);
               } else {
                  string possibleString = checkForString(currentPosition, 7);

                  if(possibleString == "Queries" && !isalnum(getCurrentChar(currentPosition + 7))) { // Grabs possible string and checks to see if it matches "Queries"
                     type = QUERIES;
                     value = possibleString;

                     currentPosition = currentPosition + 7;
                     return Token(type, value, currentLine);
                  } else { // If none of the matching strings, has to be an ID 
                     type = ID;
                     value = "";
                     int displacement = 1;
                     do {
                        value = value + string(1,currentChar);
                        int lookAtPosition = currentPosition + displacement;
                        currentChar = getCurrentChar(lookAtPosition);
                        displacement++;
                     } while(isalnum(currentChar));
                     currentPosition = currentPosition + displacement - 1;

                     return Token(type, value, currentLine);
                  }
               }
            }
         } 
      } else if(isalpha(currentChar)) { // When current char is a letter, all the following letters and numbers are part of an ID
         type = ID;
         value = "";
         int displacement = 1;
         do {
            value = value + string(1,currentChar);
            int lookAtPosition = currentPosition + displacement;
            currentChar = getCurrentChar(lookAtPosition);
            displacement++;
         } while(isalnum(currentChar));
         currentPosition = currentPosition + displacement - 1;

         return Token(type, value, currentLine);
      } else if(currentChar == '\'') { // Grabs strings when currentChar is a single quote
         
         type = STRING;
         value = "'";
         int lineNumber = currentLine;
         int displacement = 1;
         bool endLoop = false;

         do {
            currentChar = getCurrentChar(currentPosition + displacement);
            if(currentChar == '\0') { // If string isn't ended correctly then becomes UDEFINED type
               currentPosition = currentPosition + displacement;
               return Token(UNDEFINED, value, lineNumber);
               break;
            }
            if(currentChar == '\n') {
               currentLine++;
            }
            value += currentChar;
            char nextChar = getCurrentChar(currentPosition + displacement + 1);
            if(currentChar == '\'' && nextChar == '\'') { // Two single quotes does not end the string
               value += nextChar;
               displacement++;
            } else if (currentChar == '\'' && nextChar != '\'') { // String ending when finds a single quote without another right after
               endLoop = true;
            }
            displacement++;
         } while(!endLoop);
   
         currentPosition = currentPosition + displacement;
         return Token(type,value,lineNumber);         
      } else if(currentChar == '#') { // Grabs COMMENTs when current char is a hash 
         type = COMMENT;
         char nextChar = getCurrentChar(currentPosition + 1);
         int lineNumber = currentLine;
         
         if(nextChar == '|') { // Knows that it is a block comment if next char is pipe 
            value = "";
            int displacement = 1;
            do {
               value = value + string(1,currentChar);
               int lookAtPosition = currentPosition + displacement;
               currentChar = getCurrentChar(lookAtPosition);

               if(currentChar == '\n') {
                  currentLine++;
               }
               if(currentChar == '#') {
                  value = value + string(1,currentChar);

                  currentPosition = currentPosition + displacement + 1;
                  return Token(type, value, lineNumber);
               }
               if(currentChar == '\0') { // If block comment does not end correctly becomes UNDEFINED type
                  type = UNDEFINED;
                  value = "";

                  currentPosition = currentPosition + displacement;
                  return Token(type, value, lineNumber);
               }
               displacement++;
             } while (displacement != 0);
         } else { // Line comment logic
            value = "";
            int displacement = 1;
            
            do {
               value = value + string(1,currentChar);
               int lookAtPosition = currentPosition + displacement;
               currentChar = getCurrentChar(lookAtPosition);
               displacement++;
            } while(currentChar != '\n');
            
            currentPosition = currentPosition + displacement - 1;

            return Token(type, value, currentLine);
         }
      } else { // If none of the above, type is UNDEFINED
         type = UNDEFINED;
         value = currentChar;

         currentPosition++;
         return Token(type, value, currentLine);
      }

      return Token(type, value, currentLine);
   }


};
