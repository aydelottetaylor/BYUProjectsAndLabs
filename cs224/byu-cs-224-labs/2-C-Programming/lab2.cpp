#include <stdio.h>

//Main function
int main() {
   // Task 1
   char a = 'a';
   short b = 32;
   int c = 128;
   long d = 9987;
   float e = 3.14159;
   double f = 2.14e19;

   // Task 2
   printf("TASK 2\n");
   printf("location of a is: %p\n", &a);
   printf("location of b is: %p\n", &b);
   printf("location of c is: %p\n", &c);
   printf("location of d is: %p\n", &d);
   printf("location of e is: %p\n", &e);
   printf("location if f is: %p\n", &f);

   // Task 3
   int scores [5] = {1, 2, 3, 4, 5};
   char letters [5] = {'a', 'b', 'c', 'd', 'e'};

   // Task 4
   printf("\nTASK 4\n");

   for(int i = 0; i < 5; i++) {
      printf("location of scores[%d] is: %p\n", i, &scores[i]);
      printf("location of letters[%d] is: %p\n", i, &letters[i]);
   }

   // Task 5
   printf("\nPlease input 5 numbers: \n");

   double sum = 0.0;
   double entry = 0.0;
   double average = 0.0;
   for(int i = 0; i < 5; ++i) {
      scanf("%lf", &entry);
      sum+=entry;
   }
   printf("The average of the five numbers is: %lf\n", sum/5);

   // Task 6
   printf("\nPlease input 5 more numbers: \n");

   double numbers[5];
   for(int i = 0; i < 5; ++i) {
      scanf("%lf", &numbers[i]);
   }

   int swapped = 0;
   int i,j;
   for(i = 0; i < 4; i++) {
      swapped = 0;
      for(j = 0; j < 4 - i; j++) {
         if(numbers[j] > numbers[j+1]) {
            double temp = numbers[j];
            numbers[j] = numbers[j+1];
            numbers[j+1] = temp;
            swapped = 1;
         }
      }
      if(swapped = 0) {
         break;
      }
   }

   printf("The ordered numbers are:\n");
   for(int i = 0; i < 5; ++i) {
      printf("%lf\n", numbers[i]);
   }

   return 0;
}
