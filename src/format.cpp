#include "format.h"
#include <iomanip>
#include <string>

using namespace std;

string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  int minutes = (seconds / 60) % 60;
  seconds = seconds % 60;

  stringstream time;

  time << setfill('0') << setw(2) << hours << ":";
  time << setfill('0') << setw(2) << minutes << ":";
  time << setfill('0') << setw(2) << seconds;

  return time.str();
}