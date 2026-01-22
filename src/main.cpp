#include "init_window.h"

const int window_height = 480, window_width = 640;
std::string title = "hello";

#if __linux__

int main() {  
  init_x11window(100, 100, window_width, window_height, title);
  while(!x11_shouldclose()) {
    x_pollevents();  
  }
  close_x11window();
  return 0;
}
#endif

#if _WIN32

int main() {
    //HINSTANCE win32_pid = GetModuleHandle(NULL);
    init_win32window(CW_USEDEFAULT, CW_USEDEFAULT, window_height, window_width, title);
    while(!win32_shouldclose()) {
      win32_pollevents();
    }
    return 0;
}
#endif