#include <stdio.h>
#include <iostream>
#include "token.h"
#include "scanner.h"
using namespace std;

int main() {
   string input1 = " ,";

   Scanner scanner1(input1);

   Token token1 = scanner1.scanToken();

   cout << "Test Token: " << token1.toString() << " (Original was ' ,') "<< endl;

   return 0;
}
