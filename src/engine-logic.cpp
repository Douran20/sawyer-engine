#include "engine-logic.hpp"

int GetRandomValue(int min, int max)
{
  static std::mt19937 gen(std::random_device{}());
  std::uniform_int_distribution<int> dist(min, max);
  return dist(gen);
}

void msDelay(unsigned int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

double GetTime() {
  #if __linux__
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
  #endif
  #if _WIN32
  static LARGE_INTEGER frequency;
  static BOOL init = QueryPerformanceFrequency(&frequency);
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  return (double)counter.QuadPart / (double)frequency.QuadPart;
  #endif
}

int frameindex(int tfps, int xfps){
  return static_cast<int>(GetTime() * tfps) % xfps;
}