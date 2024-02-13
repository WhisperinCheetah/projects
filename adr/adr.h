#include "raylib.h"
#include <stdlib.h>

typedef struct _ADRImage {
    void* data;
    int colorCount;
    Color* colors;
    int width;
    int height;
} ADRImage;

void destroyADRImage(ADRImage* image) {
    free(image->data);
    free(image->colors);
    free(image);
}