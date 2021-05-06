#include "format.h"

#include <iomanip>
#include <stdexcept>
#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  unsigned long hour{0};
  unsigned long minute{0};
  unsigned long second{0};

  hour = seconds / 3600;
  seconds = seconds % 3600;
  minute = seconds / 60;
  second = seconds % 60;

  std::ostringstream output;
  output << std::setfill('0') << std::setw(2) << hour << ":" << std::setw(2)
         << minute << ":" << std::setw(2) << second;

  return output.str();
}