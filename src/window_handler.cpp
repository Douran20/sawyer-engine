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
  
  // this makes the window not resizeable
  XSizeHints* hints = XAllocSizeHints();
  hints->flags = PMinSize | PMaxSize;
  //this locks the width and height
  hints->min_width  = hints->max_width  = width;
  hints->min_height = hints->max_height = height;
  // Apply the hints to the window
  XSetWMNormalHints(x_display, x_window, hints);
  XFree(hints);
  
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
XImage* x_image    = nullptr;
GC        x_gc       = nullptr;
//uint32_t x_windowpixels;

void x_render_buffers(Window window, PixelFormat& pixels) {
  XRenderPictFormat *render_format = XRenderFindVisualFormat(x_display, DefaultVisual(x_display, x_screen));
  int height = xwindow_screen(window).height;
  int width = xwindow_screen(window).width;
  
  back_buffer = XCreatePixmap(x_display, x_window, width, height, DefaultDepth(x_display, x_screen));
  back_xbuffer = XRenderCreatePicture(x_display, back_buffer , render_format, 0, NULL);
  front_xbuffer = XRenderCreatePicture(x_display, x_window , render_format, 0, NULL);

  // get the pixel map of the window and pass it to our rendeder
  uint32_t* getwindowpixels = (uint32_t*)malloc(width * height * 4);
  
  pixels.pixels = getwindowpixels;
  pixels.width = width;
  pixels.height = height;
  
  x_image = XCreateImage(x_display, DefaultVisual(x_display, x_screen), 24, ZPixmap, 0, (char*) getwindowpixels, width, height, 32, 0);
  x_gc = XCreateGC(x_display, window, 0, NULL);
}
//void x_clearbackground(PixelFormat& pixels, uint32_t color) {
//    // We can use a simple loop or memset since we have the pointer
//    int total = pixels.width * pixels.height;
//    for (int i = 0; i < total; ++i) {
//        pixels.pixels[i] = color;
//    }
//}
void x_draw_buffers(Window window) {
  XPutImage(x_display, back_buffer, x_gc, x_image, 0, 0, 0, 0, xwindow_screen(window).width, xwindow_screen(window).height);
  XRenderComposite(x_display, PictOpSrc, back_xbuffer, None, front_xbuffer, 0, 0, 0, 0, 0, 0, xwindow_screen(window).width, xwindow_screen(window).height);
  XFlush(x_display);
}
void close_x11window() {
  if (x_image) {
    XDestroyImage(x_image);
    std::cout << "Destroyed Ximage\n";
  } 
  if (back_xbuffer){ 
    XRenderFreePicture(x_display, back_xbuffer);
    std::cout << "Unloaded X11 back buffer\n";
  }
  if (front_xbuffer) { 
    XRenderFreePicture(x_display, front_xbuffer);
    std::cout << "Unloaded X11 front buffer\n";
  }
  if (back_buffer) {
    XFreePixmap(x_display, back_buffer);
    std::cout << "Unloaded Client back buffer\n";
  }
  if (x_gc) XFreeGC(x_display, x_gc);
  XUnmapWindow(x_display, x_window);
  XDestroyWindow(x_display, x_window);
  // xclosedisplay for true returns 0
  XCloseDisplay(x_display);
  std::cout << "closed successfully\n";
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

HWND init_win32window(int posx, int posy, int width, int height, const char *title) {
  const char CLASS_NAME[] = "chris_sawyer";

  WNDCLASS wc = {}; // "window class"

  wc.lpfnWndProc = WndProc; //handles the window io
  wc.hInstance = win32_pid; //tell what program the window belongs to.
  wc.lpszClassName = CLASS_NAME; // pass the window class name to window
  wc.hCursor = LoadCursor(NULL, IDC_ARROW); //need this or else the cursor disappears inside of the window

  // this checks if we have a windowclass name.
  if (!RegisterClass(&wc)) {
    MessageBox(NULL, "Class Registration Failed!", "Error", MB_ICONERROR);
    exit(1);
  }

  // args (addtional sytles, window class name, title, style, posx, posy, width, height, the "hparent" prob used for subwindows, same thing again. PID, "Long Pointer Parameter")
  HWND hwnd = CreateWindowEx(0, CLASS_NAME, title, WS_OVERLAPPEDWINDOW, posx, posy, width, height, NULL, NULL, win32_pid, NULL);

  if (hwnd == NULL) {
    MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONERROR);
    exit(1);
  }

  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);
  return hwnd;
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
HDC back_buffer;
void* rawPixels;
void win32_initbuffer(HWND hwnd, PixelFormat& pixels) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;

    HDC hdc_window = GetDC(hwnd);

    // 1. Create the 'back_buffer' (The X11 Pixmap equivalent)
    back_buffer = CreateCompatibleDC(hdc_window);
    
    // 2. Create the 'x_image' equivalent (The CPU-accessible RAM)
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h; // Negative for top-down coordinates
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    // This creates the buffer and gives us the raw pointer (pixels.pixels)
    HBITMAP hBitmap = CreateDIBSection(back_buffer, &bmi, DIB_RGB_COLORS, (void**)&pixels.pixels, NULL, 0);
    
    // Select the bitmap into our DC so we can work with it
    SelectObject(back_buffer, hBitmap);

    pixels.width = w;
    pixels.height = h;

    ReleaseDC(hwnd, hdc_window);
}
void win32_drawbuffer(HWND window_handler, PixelFormat& pixels){
  PAINTSTRUCT draw;
  //HDC hdcwindow = BeginPaint(window_handler, &draw);
  HDC hdc_window = GetDC(window_handler);
  BitBlt(hdc_window, 0, 0, pixels.width, pixels.height, back_buffer, 0,0,SRCCOPY);
  //EndPaint(window_handler, &draw);
  ReleaseDC(window_handler, hdc_window);
  PatBlt(back_buffer, 0, 0, pixels.width, pixels.height, BLACKNESS);
  //std::cout << "draw buffer\n" << std::flush;
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

//int InitWindow(int posx, int posy, int width, int height, std::string title) {
//  // for some reason the window size is diff across both even tho they use the same value???
//  // also it isnt consistant on linux?
//  #if __linux__
//  return init_x11window(posx,posy,width,height,title);
//  #endif
//  #if _WIN32
//  init_win32window(posx,posy,width,height,title.c_str());
//  #endif
//}
#if __linux__
Window InitWindow(int posx, int posy, int width, int height, std::string title) {
  // for some reason the window size is diff across both even tho they use the same value???
  // also it isnt consistant on linux?
  return init_x11window(posx,posy,width,height,title);
}
#endif
#if _WIN32
HWND InitWindow(int posx, int posy, int width, int height, std::string title) {
  // for some reason the window size is diff across both even tho they use the same value???
  // also it isnt consistant on linux?
  return init_win32window(posx,posy,width,height,title.c_str());
}
#endif 
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
#if __linux__
void WindowInitBuffer(Window window, PixelFormat& pixels) {
  x_render_buffers(window, pixels);
}
#endif // __linux__
#if _WIN32
void WindowInitBuffer(HWND window, PixelFormat& pixels) {
  win32_initbuffer(window, pixels);
}
#endif// _WIN32
#if __linux__
void WindowRenderBuffer(Window window, PixelFormat& pixels){
  x_draw_buffers(window);
}
#endif
#if _WIN32
void WindowRenderBuffer(HWND window, PixelFormat& pixels){
  win32_drawbuffer(window, pixels);
}
#endif