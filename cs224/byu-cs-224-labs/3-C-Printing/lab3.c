#include <stdio.h>

int main() {
   // Task 1
   unsigned char u = 84;
   unsigned char c = 'T';
   unsigned char h = 0x54;

   // Task 2
   int num = 84;
   char letter = 'A';

   printf("The first variable things: %c, %x, %d.\n", num, num, num);
   printf("The second variable things: %c, %x, %d.\n", letter, letter, letter);

   // Task 3
   char a[9] = { 71, 111, 32, 67, 111, 117, 103, 115, 33 };

   printf("\n%s\n", a);

   // Task 4
   for(int i = 0; i < 9; ++i) {
      printf("Hex number of %c: %x\n", a[i], a[i]);
   }

   // Task 5
   unsigned char x = '!';
   char bits[8];
   printf("\n");

   for(int i = 8; i > 0; --i) {
      if(x%2 == 1) {
         bits[i] = 49;
      } else {
         bits[i] = 48;
      }
      x = x / 2;
   }
   printf("Binary: ");
   for(int i = 0; i < 9; i++) {
      printf("%c", bits[i]);
   }
   printf("\n");

   return 0;

}
