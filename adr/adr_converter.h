#ifndef _ADR_CONVERTER
#define _ADR_CONVERTER

#include "adr.h"

ADRImage* convert_to_adr(Image* image, int quality);
Image* convert_from_adr(ADRImage* adrImage, PixelFormat format);

#endif // _ADR_CONVERTER