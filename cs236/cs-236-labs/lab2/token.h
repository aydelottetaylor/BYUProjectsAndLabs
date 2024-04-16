#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <cstdio>
using namespace std;

enum TokenType {
   NEW_LINE, // The \n character
   COMMA, // The ',' character
   PERIOD, // The '.' character
   Q_MARK, // The '?' character
   LEFT_PAREN,  // The '(' character
   RIGHT_PAREN, // The ')' character
   COLON, // The ':' character
   COLON_DASH, // The string ":-"
   MULTIPLY, // The '*' character
   ADD, // The '+' character
   SCHEMES, //	The string "Schemes"
   FACTS, // The string "Facts"
   RULES, // The string "Rules"
   QUERIES, // The string "Queries"
   ID, // An identifier is a letter followed by zero or more letters or digits, 
       // and is not a keyword (Schemes, Facts, Rules, Queries).
       // Note that for the input "1stPerson" the scanner would find two tokens: 
       // an 'undefined' token made from the character "1" and an 'identifier' token 
       // made from the characters "stPerson".
   STRING, // A string is a sequence of characters enclosed in single quotes. 
           // White space (space, tab) is not skipped when inside a string. 
           // Two adjacent single quotes within a string denote an apostrophe. 
           // The line number for a string token is the line where the string begins. 
           // If a string is not terminated (end of file is encountered before the end of 
           // the string), the token becomes an undefined token.
           //
           // The 'value' of a token printed to the output is the sequence of input 
           // characters that form the token. For a string token this means that two 
           // adjacent single quotes in the input are printed as two adjacent single 
           // quotes in the output. (In other words, don't convert two adjacent single 
           // quotes in a string to just one apostrophe in the output.)
   COMMENT, // A line comment starts with a hash character (#) and ends 
            // at the end of the line or end of the file.
            //
            // A block comment starts with #| and ends with |#. Block comments may 
            // cover multiple lines. Block comments can be empty and multiple comments 
            // can appear on the same line. The line number for a comment token is the 
            // line where the comment begins. If a block comment is not terminated 
            // (end of file is encountered before the end of the comment), the token 
            // becomes an undefined token.
   UNDEFINED, // Any character not tokenized as a string, keyword, identifier, symbol, 
              // or white space is undefined. Additionally, any non-terminating string 
              // or non-terminating block comment is undefined. In both of the latter 
              // cases you reach EOF before finding the end of the string or the end 
              // of the comment.
   END_OF_FILE, // The end of the input file.
};

class Token {
private:
   TokenType type;
   string value;
   int line;

public:
   Token() : type(TokenType::END_OF_FILE), value(""), line(0) {}
   Token(TokenType type, string value, int line) : type(type), value(value), line(line) {}

   // Get Token Type for .cpp file so can tell when to stop do-while loop
   string getTokenType() const {
     return typeName(type); 
   }

   TokenType getType() const{
      return type;
   }

   // Create a string out of Token object information
   string toString() const {
      stringstream out;
      out << "(" << typeName(type) << "," << "\"" << value << "\"" << "," << line << ")";
      return out.str();
   }

   // Return a string when given a Token type from toString()
   static string typeName(TokenType type) {
      switch(type) {
         case NEW_LINE: return "NEW_LINE";
         case COMMA: return "COMMA";
         case PERIOD: return "PERIOD";
         case Q_MARK: return "Q_MARK";
         case LEFT_PAREN: return "LEFT_PAREN";
         case RIGHT_PAREN: return "RIGHT_PAREN";
         case COLON: return "COLON";
         case COLON_DASH: return "COLON_DASH";
         case MULTIPLY: return "MULTIPLY";
         case ADD: return "ADD";
         case SCHEMES: return "SCHEMES";
         case FACTS: return "FACTS";
         case RULES: return "RULES";
         case QUERIES: return "QUERIES";
         case ID: return "ID";
         case STRING: return "STRING";
         case COMMENT: return "COMMENT";
         case UNDEFINED: return "UNDEFINED";
         case END_OF_FILE: return "EOF";
         default: return "UNKNOWN_TYPE";
      }
   }
};