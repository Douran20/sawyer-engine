#define INITWIDNOW_H
#ifndef INITWINDOW_H
#include <iostream>
#include "engine-logic.hpp"
#if __linux__
  #include <X11/Xlib.h>
  #include <X11/extensions/Xrandr.h>
  #include <X11/extensions/Xrender.h>
#endif // __linux__
#if _WIN32
  #include <windows.h>
#endif // _WIN32


int InitWindow(int posx, int posy, int width, int height,  std::string title); // returns window id
void WindowPollEvents();
bool WindowShouldClose();
void WindowClose();
int GetMonitorWidth();
int GetMonitorHeight();
void WindowRenderBuffer(Window window);
void WindowClearBuffer(Window window);
#endif //INITWIDNOW_H