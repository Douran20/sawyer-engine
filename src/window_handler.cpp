#include "window_handler.h"

#if __linux__

static bool x11isrunning = true;
// x_display is io for the x11 service.
// it sends request to x11 and x11 execute these request.
// so it can handle userinputs and also draws to the screen
Display *x_display;
Window x_window;
// its in the name. it collects events from the users and ?? sends them to the
// display.
XEvent x_event;
// x_screen is monitor
int x_screen;

// if system is on linux use the x11 service
// later need to figure out a setup to be able to draw on the window
void init_x11window(int posx, int posy, int width, int height, std::string title) {
  x_display = XOpenDisplay(NULL);
  if (x_display == NULL) {
    std::cout << stderr << "Cannot open Xdisplay\n";
    exit(1);
  }

  // this means what ever window the user is activately on.
  // so we tell x11 to make a window on the active display.
  x_screen = DefaultScreen(x_display);
  if (x_screen == 1) { // idk if DefaultScreen returns a true or false or if it
                       // returns what screen its on
    std::cout << stderr << " - Cannot Grap Current Screen\n";
    exit(1);
  }

  XSetWindowAttributes x_attributes;
  x_attributes.background_pixel = WhitePixel(x_display, x_screen);
  x_attributes.border_pixel = BlackPixel(x_display, x_screen);
  x_attributes.event_mask = ExposureMask | KeyPressMask | ButtonPressMask;

  unsigned long x_mask = CWBackPixel | CWBorderPixel | CWEventMask;

  // XCreateWindow returns the window id
  x_window = XCreateWindow(
      x_display, RootWindow(x_display, x_screen), posx, posy, width, height, 1,
      DefaultDepth(x_display, x_screen), InputOutput,
      DefaultVisual(x_display, x_screen), x_mask, &x_attributes);
  if (x_window == 0) {
    std::cout << "Failed to create a window";
    exit(1);
  }

  XStoreName(x_display, x_window, title.c_str()); // this is window title
  XMapWindow(x_display, x_window);        // this displays the window
}

void x_pollevents() {
  // a pointer so i can say later to if x thing happens close program
  // successfully
  Atom wmDeleteMessage = XInternAtom(x_display, "WM_DELETE_WINDOW", 0);
  XSetWMProtocols(x_display, x_window, &wmDeleteMessage, 1);
  while (XNextEvent(x_display, &x_event) == 0) {
    switch (x_event.type) {
    case ClientMessage:
      if ((Atom)x_event.xclient.data.l[0] == wmDeleteMessage) {
        x11isrunning = false;
        return;
      }
    }
  }
}

void close_x11window() {
  XUnmapWindow(x_display, x_window);
  XDestroyWindow(x_display, x_window);
  // xclosedisplay for true returns 0
  XCloseDisplay(x_display);
  std::cout << "closed successfully";
  exit(0);
}

bool x11_shouldclose() {
  return !x11isrunning;
}
#endif
#if _WIN32
/*
 * windows lingo.
 * HWND is the window handle or better known as the window id.
 * Hinstance is diff from HWND. hinstance is the PID(program id).
 * wndproc is io
 */
 
static bool win32isrunning = true;
HINSTANCE win32_pid = GetModuleHandle(NULL);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
  case WM_CLOSE:
    win32isrunning = false;
    return 0;
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  default:
    return DefWindowProc(hwnd, msg, wParam, lParam);
  }
}

void init_win32window(int posx, int posy, int width, int height, const char *title) {
  const char CLASS_NAME[] = "chris_sawyer";

  WNDCLASS wc = {}; // "window class"

  wc.lpfnWndProc = WndProc; //handles the window io
  wc.hInstance = win32_pid; //tell what program the window belongs to.
  wc.lpszClassName = CLASS_NAME; // pass the window class name to window
  wc.hCursor = LoadCursor(NULL, IDC_ARROW); //need this or else the cursor disappears inside of the window

  // this checks if we have a windowclass name.
  if (!RegisterClass(&wc)) {
    MessageBox(NULL, "Class Registration Failed!", "Error", MB_ICONERROR);
    return;
  }

  // args (addtional sytles, window class name, title, style, posx, posy, width, height, the "hparent" prob used for subwindows, same thing again. PID, "Long Pointer Parameter")
  HWND hwnd = CreateWindowEx(0, CLASS_NAME, title, WS_OVERLAPPEDWINDOW, posx, posy, width, height, NULL, NULL, win32_pid, NULL);

  if (hwnd == NULL) {
    MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONERROR);
    return;
  }

  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);
}

void win32_pollevents() {
  MSG msg = {};
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    if (msg.message == WM_QUIT) {
      exit(1);
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

bool win32_shouldclose() {
    return !win32isrunning;
}

#endif