#include <windows.h>

int main() {
  // Syntax: MessageBox(WindowOwner, Text, Title, Icon/Buttons)
  MessageBox(
      NULL, 
      "If you see this, your WinAPI setup on Linux is working!", 
      "WinAPI Cross-Compile Test", 
      MB_OK | MB_ICONINFORMATION
  );
  return 0;
}