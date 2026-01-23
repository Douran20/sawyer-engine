// * TODO - implentent my own way of grabbing the monitor res so we dont need a x and win32 way of getting res.
// * TODO - need inputs
// DONE * TODO - i need to figure out how to draw into the window
// added buffer implentation for drawing.
#include "window_handler.hpp"

#if __linux__
static bool x11isrunning = true;
// x_display is io for the x11 service.
// it sends request to x11 and x11 execute these request.
// so it can handle userinputs and also draws to the screen
Display *x_display = XOpenDisplay(NULL);
Window x_window;
// its in the name. it collects events from the users and ?? sends them to the
// display.
XEvent x_event;
// x_screen is monitor
int x_screen = DefaultScreen(x_display);
Window x_root = DefaultRootWindow(x_display);

// if system is on linux use the x11 service
Window init_x11window(int posx, int posy, int width, int height, std::string title) {
  //x_display = XOpenDisplay(NULL);
  if (x_display == NULL) {
    std::cout << "Cannot open Xdisplay\n";
    exit(1);
  }

  // this means what ever window the user is activately on.
  // so we tell x11 to make a window on the active display.
  //x_screen = DefaultScreen(x_display);
  if (x_screen == 1) { // idk if DefaultScreen returns a true or false or if it
                       // returns what screen its on
    std::cout << " - Cannot Grap Current Screen\n";
    exit(1);
  }

  XSetWindowAttributes x_attributes;
  x_attributes.background_pixel = WhitePixel(x_display, x_screen);
  x_attributes.border_pixel = BlackPixel(x_display, x_screen);
  x_attributes.event_mask = ExposureMask | KeyPressMask | ButtonPressMask;

  unsigned long x_mask = CWBackPixel | CWBorderPixel | CWEventMask;

  // XCreateWindow returns the window id
  x_window = XCreateWindow(
      x_display, x_root, posx, posy, width, height, 1,
      DefaultDepth(x_display, x_screen), InputOutput,
      DefaultVisual(x_display, x_screen), x_mask, &x_attributes);
  if (x_window == 0) {
    std::cout << "Failed to create a window\n";
    exit(1);
  }
  std::cout << "Window ID = " << x_window << std::endl << std::flush;
  XStoreName(x_display, x_window, title.c_str()); // this is window title
  XSelectInput(x_display, x_window, ExposureMask);
  XMapWindow(x_display, x_window);        // this displays the window
  
  return x_window;
}

// before for somereason x_pollevents caused the os to freeze when ran.
// the suspect is prob the atom if i had to guess.
static void x_pollevents() {
  // a pointer so i can say later to if x thing happens close program
  // successfully
 	Atom wmDeleteMessage = XInternAtom(x_display, "WM_DELETE_WINDOW", false);
  int x_protocol = XSetWMProtocols(x_display, x_window, &wmDeleteMessage, 1);
  if (x_protocol < 0) {
    std::cout << "FAILED TO CREATE PROTOCOLS\n";
    exit(1);
  }
  while (XPending(x_display) > 0)
 	{
    XNextEvent(x_display, &x_event);
    switch (x_event.type)
		{
 			//case ButtonPress:
        //  x11isrunning = false;
      return;
      case ClientMessage:
        if ((Atom)x_event.xclient.data.l[0] == wmDeleteMessage) 
        {
          x11isrunning = false;
          return;
        }
        break;
		}
 	}
}
XWindowAttributes xwindow_screen(Window window){
  XWindowAttributes window_screen;
  XGetWindowAttributes(x_display, window, &window_screen);
  return window_screen;
}
  // pictures to my understanding is how you draw to the window.
  Pixmap back_buffer;
  Picture back_xbuffer;
  Picture front_xbuffer;

void x_render_buffers(Window window) {
  XRenderPictFormat *render_format = XRenderFindVisualFormat(x_display, DefaultVisual(x_display, x_screen));
  back_buffer = XCreatePixmap(x_display, x_window, xwindow_screen(window).width, xwindow_screen(window).height, DefaultDepth(x_display, x_screen));
  
  back_xbuffer = XRenderCreatePicture(x_display, back_buffer , render_format, 0, NULL);
  front_xbuffer = XRenderCreatePicture(x_display, x_window , render_format, 0, NULL);
  
  XRenderColor background = {0x00, 0x00, 0x00, 0x00}; // black
  XRenderFillRectangle(x_display, PictOpSrc, back_xbuffer, &background, 0, 0, xwindow_screen(window).width, xwindow_screen(window).height);
}
void x_clear_buffers(Window window) {
  XRenderComposite(x_display, PictOpSrc, back_xbuffer, None, front_xbuffer, 0, 0, 0, 0, 0, 0, xwindow_screen(window).width, xwindow_screen(window).height);
  XFlush(x_display);
}
void close_x11window() {
  XUnmapWindow(x_display, x_window);
  XDestroyWindow(x_display, x_window);
  // xclosedisplay for true returns 0
  XCloseDisplay(x_display);
  exit(0);
}
bool x11_shouldclose() {
  return !x11isrunning;
}
int X11GetMonitorWidth() {
  XRRScreenResources* res = XRRGetScreenResourcesCurrent(x_display, x_root);
  XRRCrtcInfo* crtc = XRRGetCrtcInfo(x_display, res, res->crtcs[0]);
  int width = crtc->width;
  
  XRRFreeCrtcInfo(crtc);
  XRRFreeScreenResources(res);
  return width;
}
int X11GetMonitorHeight() {
  XRRScreenResources* res = XRRGetScreenResourcesCurrent(x_display, x_root);
  XRRCrtcInfo* crtc = XRRGetCrtcInfo(x_display, res, res->crtcs[0]);
  int height = crtc->height;
  
  XRRFreeCrtcInfo(crtc);
  XRRFreeScreenResources(res);
  return height;
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

void win32_renderbuffer(){
  
}

void win32_clearbuffer(){
  
}

int Win32GetMonitorWidth() {
  HMONITOR hMonitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
  MONITORINFO info = {};
  info.cbSize = sizeof(MONITORINFO);
  
  if (GetMonitorInfo(hMonitor, &info)) {
    int width = info.rcMonitor.right - info.rcMonitor.left;
    return width;
  }
  exit(1);
}
int Win32GetMonitorHeight() {
  HMONITOR hMonitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
  MONITORINFO info = {};
  info.cbSize = sizeof(MONITORINFO);
  
  if (GetMonitorInfo(hMonitor, &info)) {
    int height = info.rcMonitor.bottom - info.rcMonitor.top;
    return height;
  }
  exit(1);
}
#endif

int InitWindow(int posx, int posy, int width, int height, std::string title) {
  // for some reason the window size is diff across both even tho they use the same value???
  // also it isnt consistant on linux?
  #if __linux__
  return init_x11window(posx,posy,width,height,title);
  #endif
  #if _WIN32
  init_win32window(posx,posy,width,height,title.c_str());
  #endif
}
void WindowPollEvents() {
  #if __linux__
  x_pollevents();
  #endif
  #if _WIN32
  win32_pollevents();
  #endif
}
bool WindowShouldClose() {
  #if __linux__
  return !x11isrunning;
  #endif
  #if _WIN32
  return !win32isrunning;
  #endif
}
void WindowClose() {
  std::cout << "closed successfully\n";
  SawyerDelay(0.05f); // 50 ms
  #if __linux__
  close_x11window();
  #endif
}
int GetMonitorWidth() {
  #if __linux__
  return X11GetMonitorWidth();
  #endif
  #if _WIN32
  return Win32GetMonitorWidth();
  #endif
}
int GetMonitorHeight() {
  #if __linux__
  return X11GetMonitorHeight();
  #endif
  #if _WIN32
  return Win32GetMonitorHeight();
  #endif
}
void WindowRenderBuffer(Window window) {
  #if __linux__
  x_render_buffers(window);
  #endif
}
void WindowClearBuffer(Window window) {
  #if __linux__
  x_clear_buffers(window);
  #endif
}