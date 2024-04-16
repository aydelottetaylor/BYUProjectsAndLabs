#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0
#define BAD_NUMBER_ARGS 1

/**
 * Parses the command line.
 *
 * argc: the number of items on the command line (and length of the
 *       argv array) including the executable
 * argv: the array of arguments as strings (char* array)
 * bits: the integer value is set to TRUE if bits output indicated
 *       outherwise FALSE for hex output
 *
 * returns the input file pointer (FILE*)
 **/
FILE *parseCommandLine(int argc, char **argv, int *bits) {
  if (argc > 2) {
    printf("Usage: %s [-b|-bits]\n", argv[0]);
    exit(BAD_NUMBER_ARGS);
  }

  if (argc == 2 &&
      (strcmp(argv[1], "-b") == 0 || strcmp(argv[1], "-bits") == 0)) {
    *bits = TRUE;
  } else {
    *bits = FALSE;
  }

  return stdin;
}

/**
 * Writes data to stdout in hexadecimal.
 *
 * See myxxd.md for details.
 *
 * data: an array of no more than 16 characters
 * size: the size of the array
 **/
void printDataAsHex(unsigned char *data, size_t size) {
  //printf("TODO 1: printDataAsHex (2)");
  for (size_t i = 0; i < size; ++i) {
     if(i%2 == 0) {
       printf(" ");
     }
     printf("%02x", data[i]);
  }

  for (size_t i = size; i < 16; ++i) {
    if(i%2 == 0) {
      printf("   ");
    } else {
      printf("  ");
    }
  }
}

/**
 * Writes data to stdout as characters.
 *
 * See myxxd.md for details.
 *
 * data: an array of no more than 16 characters
 * size: the size of the array
 **/
void printDataAsChars(unsigned char *data, size_t size) {
  //printf("TODO 2: printDataAsChars (3)");
  for (size_t i = 0; i < size; ++i) {
    printf("%c", (isprint(data[i]) ? data [i] : '.'));
  }
}

void readAndPrintInputAsHex(FILE *input) {
  unsigned char data[16];
  int numBytesRead = fread(data, 1, 16, input);
  unsigned int offset = 0;
  while (numBytesRead != 0) {
    printf("%08x:", offset);
    offset += numBytesRead;
    printDataAsHex(data, numBytesRead);
    printf("  ");
    printDataAsChars(data, numBytesRead);
    printf("\n");
    numBytesRead = fread(data, 1, 16, input);
  }
}

void printDataAsCharsForBits(unsigned char *data, size_t size) {
  //printf("TODO 2: printDataAsChars (3)");
  for (size_t i = 0; i < size; ++i) {
    printf("%c", (isprint(data[i]) ? data [i] : '.'));
  }
}

void printDataAsBits(unsigned char *data, size_t size) {
  for (int i = 0; i < size; ++i) {
    for (int j = 7; j >= 0; --j) {
      printf("%d", (data[i] >> j) &1);
    }
    printf(" ");
  }
  for (int i = size; i < 6; ++i) {
    printf("         ");
  }
}

/**
 * Bits output for xxd.
 *
 * See myxxd.md for details.
 *
 * input: input stream
 **/
void readAndPrintInputAsBits(FILE *input) {
  //printf("TODO 3: readAndPrintInputAsBits\n");
  unsigned char data[16];
  int numBytesRead = fread(data, 1, 6, input);
  unsigned int offset = 0;

  while (numBytesRead !=0) {
    printf("%08x: ", offset);
    offset += numBytesRead;
    printDataAsBits(data, numBytesRead);
    printf(" ");
    printDataAsCharsForBits(data, numBytesRead);
    printf("\n");
    numBytesRead = fread(data, 1, 6, input);
  }
}

int main(int argc, char **argv) {
  int bits = FALSE;
  FILE *input = parseCommandLine(argc, argv, &bits);

  if (bits == FALSE) {
    readAndPrintInputAsHex(input);
  } else {
    readAndPrintInputAsBits(input);
  }
  return 0;
}
