#if _WIN32
#include <windows.h>
#endif
#include <random>
#include <thread>
#include <chrono>
#if __linux__
#include <time.h>
#endif
void msDelay(unsigned int milliseconds);
double GetTime();
int frameindex(int tfps, int xfps);
int GetRandomValue(int min, int max);