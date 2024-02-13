# ADR lossy image format

### Image format:
- void data
- int color_count
- Color* colors
- int width
- int height

To convert an image, one must provide a quality value between 100 and 0. With 100 being no compression, 1 being as much compression as possible and 0 being auto.

### Converter:
- converts ADR -> other
- converts other -> ADR

#### Exported functions:
- ADRImage* convert_to_adr(Image* image, int quality)
- Image* convert_from_adr(ADRImage* adrImage, PixelFormat format)

### Viewer: 
- Opens image in new window

### Loader (adr.h):
- Loads image and puts it in heap
- Proper destructor

## COMPRESSION IDEA
Use k-means clustering to reduce amount of colors. Pre define the x amount of used colors and convert every pixel to this compressed format.

### Options:
- (min/max/auto) color count
- convert/view
