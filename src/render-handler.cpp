#include "render-handler.hpp"

void DrawPixel(PixelFormat& pixels, int posx, int posy, uint32_t color) {
  // boundry check
  if ( posx < 0 || posx >= pixels.width || posy < 0 || posy >= pixels.height) return;  
  
  // collum rendering
  // ---------------->
  // ---------------->
  // drawing from left to right for every row of pixels.
  pixels.pixels[posy * pixels.width + posx] = color;
}

void DrawRectangle(PixelFormat& pixels, int posx, int posy, int width, int height, uint32_t color) {
  if (!pixels.pixels) return;
  
  for (int drawy = posy; drawy < posy + height; ++drawy) {
    for (int drawx = posx; drawx < posx + width; ++drawx){
      DrawPixel(pixels, drawx, drawy, color);
    }
  }
}

void ClearBackground(PixelFormat& pixels, uint32_t color) {
  int total = pixels.width * pixels.height;
  for (int i = 0; i < total; ++i) {
      pixels.pixels[i] = color;
  }
}