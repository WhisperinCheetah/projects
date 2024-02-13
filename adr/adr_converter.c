#include "adr_converter.h"

ADRImage* convert_to_adr(Image* image, int quality) {
    if (image->format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8) {
        ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    }

    
}