// LINUX for some reason is very precular with the ordering of the lines.

#include "sawyer-engine.hpp"
#include "engine-logic.hpp"
//#include <ostream>

const int screen_width = GetMonitorWidth(), screen_height = GetMonitorHeight();
const int window_width = 1920, window_height = 1080;

// this will spawn the window in the center of the screen correctly (atleast on the window side of things)
const int posx = screen_width/2 - window_width/2, posy = screen_height/2 - window_height/2;
std::string title = "sawyer-engine";

void engine() {
  std::cout << posx << std::endl << posy << std::endl;
  std::cout << screen_width << std::endl << screen_height << std::endl;
  InitWindow(posx, posy, window_width, window_height, title);
  while(!WindowShouldClose()) {
    //std::cout << frameindex(30, 30) << std::endl << std::flush;
    std::cout << GetRandomValue(0, 100) << std::endl <<std::flush;
    msDelay(50);
    WindowPollEvents();
  }
  WindowClose();
  //fprintf(stderr, "X11 Width: %d\n", screen_width);
  exit(0);
}