#include <iomanip>
#include <sstream>
#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long hours = seconds / 3600;
  long mins = (seconds % 3600) / 60;
  long secs = seconds - 3600 * hours - 60 * mins;
  std::ostringstream osstrm;
  osstrm << std::setw(2) << std::setfill('0') << std::to_string(hours) << ":"
         << std::setw(2) << std::setfill('0') << std::to_string(mins) << ":"
         << std::setw(2) << std::setfill('0') << std::to_string(secs);
  return osstrm.str();
}