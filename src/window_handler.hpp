#define INITWIDNOW_H
#ifndef INITWINDOW_H
#include <iostream>

void InitWindow(int posx, int posy, int width, int height,  std::string title);
void WindowPollEvents();
bool WindowShouldClose();
void WindowClose();
int GetMonitorWidth();
int GetMonitorHeight();

#if __linux__
  #include <X11/Xlib.h>
  #include <X11/extensions/Xrandr.h>
#endif
#if _WIN32
  #include <windows.h>
#endif
#endif