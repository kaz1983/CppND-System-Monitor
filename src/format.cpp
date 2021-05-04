#include <iomanip>
#include <stdexcept>
#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    unsigned long hour{0};
    unsigned long minute{0};
    unsigned long second{0};

    hour = seconds/3600;
    seconds = seconds%3600;
    minute = seconds/60;
    second = seconds%60;

    std::ostringstream output;
    output << std::setfill('0') << std::setw(2) << hour << ":" << std::setw(2) << minute << ":" << std::setw(2) << second;
    
    return output.str(); 
}