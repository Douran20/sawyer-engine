#define INITWIDNOW_H
#ifndef INITWINDOW_H
#include <iostream>

#if __linux__
  #include <X11/Xlib.h>
  
  void init_x11window(int posx, int posy, int width, int height,  std::string title);
  void x_pollevents();
  void close_x11window();
  bool x11_shouldclose();
#endif
#if _WIN32
  #include "windows.h"
  #include <stdint.h>
  
  void init_win32window(int posx, int posy, int width, int height, const char *title);
  void win32_pollevents();
  bool win32_shouldclose();
#endif
#endif