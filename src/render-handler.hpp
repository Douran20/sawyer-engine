#include <cstdint>

// this is our pixels
// it contains the color of the pixels
// the width and height of the pixels
struct PixelFormat {
    int width;
    int height;
    //pixels store the color
    uint32_t* pixels;
};

void DrawRectangle(PixelFormat& pixels, int posx, int posy, int width, int height, uint32_t color);
void ClearBackground(PixelFormat& pixels, uint32_t color);