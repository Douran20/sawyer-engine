#define INITWIDNOW_H
#ifndef INITWINDOW_H
#include <iostream>
#include <cstdint>
#include "render-handler.hpp"
#include "engine-logic.hpp"
#if __linux__
  #include <X11/Xlib.h>
  #include <X11/extensions/Xrandr.h>
  #include <X11/extensions/Xrender.h>
  #include <X11/Xutil.h>
  void WindowInitBuffer(Window window, PixelFormat& pixels);
  void WindowRenderBuffer(Window window, PixelFormat& pixels);
  Window InitWindow(int posx, int posy, int width, int height, std::string title);
#endif // __linux__
#if _WIN32
  #include <windows.h>
  void WindowInitBuffer(HWND window, PixelFormat& pixels);
  void WindowRenderBuffer(HWND window, PixelFormat& pixels);
  HWND InitWindow(int posx, int posy, int width, int height, std::string title);
#endif // _WIN32

//uint32_t* x_pixels(Window window);
//uint32_t* pixel_data = nullptr;
// 
void WindowPollEvents();
bool WindowShouldClose();
void WindowClose();
int GetMonitorWidth();
int GetMonitorHeight();
#endif //INITWIDNOW_H