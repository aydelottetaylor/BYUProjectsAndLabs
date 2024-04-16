#include <stdio.h>
#include <iostream>
#include <fstream>
#include "token.h"
#include "scanner.h"
#include <cstdio>

using namespace std;

// Fetch input from a given filename and return it in a string
// 
// Taylor Aydelotte - 1/29/2024
string getInput(string inputFileName) {
   ifstream file(inputFileName);
   if(!file.is_open()) {
      cerr << "Error opening file: " << inputFileName << endl;
   }
   // Read input file contents
   string input((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
   file.close();

   return input;
}

int main(int argc, char *argv[]) {
   
   if(argc != 2) {
      cerr << "Make sure that you include the name of the file to scan. Ex. ./main docToScan.txt" << endl;
      return 1;
   }

   string inputFileName = argv[1];
   string input = getInput(inputFileName);

   Scanner scanner(input);
   Token token(TokenType::END_OF_FILE, "", 0);

   int totalTokens = 0;

   do {
      token = scanner.scanToken();
      if(token.getTokenType() != "NEW_LINE") {
         cout << token.toString() << endl;
         totalTokens++;
      }
   } while (token.getTokenType() != "EOF");

   cout << "Total Tokens = " << totalTokens << endl;
   return 0;
}
