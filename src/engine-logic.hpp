#if _WIN32
#include <windows.h>
#endif
#include <random>
#include <thread>
#include <chrono>
#include <iostream>
#if __linux__
#include <time.h>
#endif
void SawyerDelay(float seconds);
double GetTime();
int frameindex(int tfps, int xfps);
// this just loops over rngtable()
int GetRandomValue();
// DO NOT SET THE TARGET FPS OUTSIDE OF THE RENDER LOOP
void TargetFps(float tfps);
void FpsInfo();