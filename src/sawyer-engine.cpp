// LINUX for some reason is very precular with the ordering of the lines.

#include "sawyer-engine.hpp"

const int screen_width = GetMonitorWidth(), screen_height = GetMonitorHeight();
const int window_width = 640, window_height = 480;

const int posx = screen_width/2 - window_width/2, posy = screen_height/2 - window_height/2;
std::string title = "sawyer-engine";

void engine() {
  std::cout << posx << std::endl << posy << std::endl;
  std::cout << screen_width << std::endl << screen_height << std::endl;
  int window = InitWindow(posx, posy, window_width, window_height, title);
  
  // ALWAYS ALWAYS MAKE THE LOOP SLEEP YOU FUCKING IDIOT
  while(!WindowShouldClose()) {
    WindowPollEvents();
    WindowRenderBuffer(window);
    
    
    
    WindowClearBuffer(window);
    TargetFps(60);
  }
  WindowClose();
  exit(0);
}

// moving the pollevents outside of the loop with protocal on with a delay of 3 seconds doesnt crash.
// moving the pollevents outside of the loop with protocal on doesnt crash.
// pollevents in render loop with a delay of 1 second above pollevents doesnt crash.
// the problem was i guess we tried to make the protocal way way too fast and because of that we tried to send messages to the protocal before it was created?.