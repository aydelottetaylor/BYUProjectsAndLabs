#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE  1
#define FALSE 0

#define BAD_NUMBER_ARGS 1
#define BAD_OPTION 2
#define FSEEK_ERROR 3
#define FREAD_ERROR 4
#define MALLOC_ERROR 5
#define FWRITE_ERROR 6

/**
 * Parses the command line.
 *
 * argc:      the number of items on the command line (and length of the
 *            argv array) including the executable
 * argv:      the array of arguments as strings (char* array)
 * grayscale: the integer value is set to TRUE if grayscale output indicated
 *            outherwise FALSE for threshold output
 *
 * returns the input file pointer (FILE*)
 **/
FILE *parseCommandLine(int argc, char **argv, int *isGrayscale, int *isScaleDown) {
  if (argc > 2) {
    fprintf(stderr, "Usage: %s [-g]\n", argv[0]);
    exit(BAD_NUMBER_ARGS);
  }

  if (argc == 2) {
    if (strcmp(argv[1], "-g") == 0) {
      *isGrayscale = TRUE;

    } else if (strcmp(argv[1], "-s") == 0) {
      *isScaleDown = TRUE;
    } else {
      fprintf(stderr, "Unknown option: '%s'\n", argv[1]);
      fprintf(stderr, "Usage: %s [-g]\n", argv[0]);
      exit(BAD_OPTION);  
    }
  }

  return stdin;
}

unsigned getFileSizeInBytes(FILE* stream) {
  unsigned fileSizeInBytes = 0;
  
  rewind(stream);
  if (fseek(stream, 0L, SEEK_END) != 0) {
    exit(FSEEK_ERROR);
  }
  fileSizeInBytes = ftell(stream);

  return fileSizeInBytes;
}

void getBmpFileAsBytes(unsigned char* ptr, unsigned fileSizeInBytes, FILE* stream) {
  rewind(stream);
  if (fread(ptr, fileSizeInBytes, 1, stream) != 1) {
#ifdef DEBUG
    printf("feof() = %x\n", feof(stream));
    printf("ferror() = %x\n", ferror(stream));
#endif
    exit(FREAD_ERROR);
  }
}

unsigned char getAverageIntensity(unsigned char blue, unsigned char green, unsigned char red) {
  unsigned int averageIntensity = (blue + green + red) / 3;

  if(averageIntensity > 255) {
    return 255;
  } else {
    return (unsigned char)averageIntensity;
  }
}

void applyGrayscaleToPixel(unsigned char* pixel) {
  int averageIntensity = getAverageIntensity(pixel[0], pixel[1], pixel[2]);

  pixel[0] = averageIntensity;
  pixel[1] = averageIntensity;
  pixel[2] = averageIntensity;
}

void applyThresholdToPixel(unsigned char* pixel) {
  int averageIntensity = getAverageIntensity(pixel[0], pixel[1], pixel[2]);

  if(averageIntensity >= 128){
    pixel[0] = 0xff;
    pixel[1] = 0xff;
    pixel[2] = 0xff;
  } else {
    pixel[0] = 0x00;
    pixel[1] = 0x00;
    pixel[2] = 0x00;
  }
}

void applyFilterToPixel(unsigned char* pixel, int isGrayscale) {
  if(isGrayscale) {
    applyGrayscaleToPixel(pixel);
  } else if (!isGrayscale) {
    applyThresholdToPixel(pixel);
  }
}

void applyFilterToRow(unsigned char* row, int width, int isGrayscale) {
  for (int pixel = 0; pixel < width; pixel++) {
    unsigned char* currentPixel = row + (pixel * 3);

    applyFilterToPixel(currentPixel, isGrayscale);
  }
}

void applyFilterToPixelArray(unsigned char* pixelArray, int width, int height, int isGrayscale) {
  int padding = width % 4;

#ifdef DEBUG
  printf("padding = %d\n", padding);
#endif  
  for (int row = 0; row < height; row++) {
    unsigned char* currentRow = pixelArray + (row * ((width * 3) + padding));

    applyFilterToRow(currentRow, width, isGrayscale);
  }
}

void applyScaleDownToArray(unsigned char* pixelArray, int width, int height) {
  int newWidth = width / 2;
  int newHeight = height / 2;

  printf("Scaling down image");
  
  for (int row = 0; row < newHeight; row++) {
    for (int col = 0; col < newWidth; col++) {

      int pixel1Index = ((row * 2) * width + (col * 2)) * 3;
      int pixel2Index = pixel1Index + 3;
      int pixel3Index = pixel1Index + (width * 3);
      int pixel4Index = pixel3Index + 3;

      unsigned char avgRed = (pixelArray[pixel1Index] + pixelArray[pixel2Index] + pixelArray[pixel3Index] + pixelArray[pixel4Index]) / 4;
      unsigned char avgGreen = (pixelArray[pixel1Index + 1] + pixelArray[pixel2Index + 1] + pixelArray[pixel3Index + 1] + pixelArray[pixel4Index + 1]) / 4;
      unsigned char avgBlue = (pixelArray[pixel1Index + 2] + pixelArray[pixel2Index + 2] + pixelArray[pixel3Index + 2] + pixelArray[pixel4Index + 2]) / 4;

      int newIndex = (row * newWidth + col) * 3;
      pixelArray[newIndex] = avgRed;
      pixelArray[newIndex + 1] = avgGreen;
      pixelArray[newIndex + 2] = avgBlue;
    }
  }
}

void parseHeaderAndApplyFilter(unsigned char* bmpFileAsBytes, int isGrayscale, int isScaleDown) {
  int offsetFirstBytePixelArray = 0;
  int width = 0;
  int height = 0;
  unsigned char* pixelArray = NULL;

  offsetFirstBytePixelArray = *(int *)(bmpFileAsBytes + 10);
  width = *(int *)(bmpFileAsBytes + 18);
  height = *(int *)(bmpFileAsBytes + 22);
  pixelArray = bmpFileAsBytes + offsetFirstBytePixelArray;

#ifdef DEBUG
  printf("offsetFirstBytePixelArray = %u\n", offsetFirstBytePixelArray);
  printf("width = %u\n", width);
  printf("height = %u\n", height);
  printf("pixelArray = %p\n", pixelArray);
#endif
  if(!isScaleDown) {
    applyFilterToPixelArray(pixelArray, width, height, isGrayscale);
  } else if(isScaleDown) {
    applyScaleDownToArray(pixelArray, width, height);
  }
}

int main(int argc, char **argv) {
  int grayscale = FALSE;
  int scaleDown = FALSE;
  unsigned fileSizeInBytes = 0;
  unsigned char* bmpFileAsBytes = NULL;
  FILE *stream = NULL;
  
  stream = parseCommandLine(argc, argv, &grayscale, &scaleDown);
  fileSizeInBytes = getFileSizeInBytes(stream);

#ifdef DEBUG
  printf("fileSizeInBytes = %u\n", fileSizeInBytes);
#endif

  bmpFileAsBytes = (unsigned char *)malloc(fileSizeInBytes);
  if (bmpFileAsBytes == NULL) {
    exit(MALLOC_ERROR);
  }
  getBmpFileAsBytes(bmpFileAsBytes, fileSizeInBytes, stream);
  
  parseHeaderAndApplyFilter(bmpFileAsBytes, grayscale, scaleDown);

#ifndef DEBUG
  if (fwrite(bmpFileAsBytes, fileSizeInBytes, 1, stdout) != 1) {
    exit(FWRITE_ERROR);
  }
#endif

  free(bmpFileAsBytes);
  return 0;
}