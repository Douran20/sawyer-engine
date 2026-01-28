// LINUX for some reason is very precular with the ordering of the lines.

#include "sawyer-engine.hpp"
const int screen_width = GetMonitorWidth(), screen_height = GetMonitorHeight();
const int window_width = 640, window_height = 480;
//uint32_t* pixel_data2 = nullptr;
const int posx = screen_width/2 - window_width/2, posy = screen_height/2 - window_height/2;
std::string title = "sawyer-engine";
PixelFormat bob;

void engine() {
  
  auto window = InitWindow(posx, posy, window_width, window_height, title);
  int y_pos = 50;
  int x_pos = 50;
  float direction = 4;
  int size = 50;
  std::cout << posx << std::endl << posy << std::endl;
  std::cout << screen_width << std::endl << screen_height << std::endl;
  WindowInitBuffer(window, bob);
  // ALWAYS ALWAYS MAKE THE LOOP SLEEP YOU FUCKING IDIOT
  while(!WindowShouldClose()) {
    WindowPollEvents();
    //float green_y_pos = GetRandomValue() * 5;
    //float green_x_pos = GetRandomValue() * 5;
    y_pos += direction;
    x_pos += direction;
    
    if (y_pos >= bob.height - size || y_pos <= 0) {
      direction = -direction;
    }
    if (x_pos >= bob.width - size || x_pos <= 0) {
      direction = -direction;
    }
    
    ClearBackground(bob, 0x112233);
    DrawRectangle(bob, 50, y_pos, size, size, 0xFFFF88);
    DrawRectangle(bob, x_pos, 50, size, size, 0x88FFFF);
    WindowRenderBuffer(window, bob);
    TargetFps(300);
  }
  WindowClose();
  exit(0);
}

// moving the pollevents outside of the loop with protocal on with a delay of 3 seconds doesnt crash.
// moving the pollevents outside of the loop with protocal on doesnt crash.
// pollevents in render loop with a delay of 1 second above pollevents doesnt crash.
// the problem was i guess we tried to make the protocal way way too fast and because of that we tried to send messages to the protocal before it was created?.