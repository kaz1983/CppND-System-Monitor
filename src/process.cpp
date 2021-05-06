#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid)
    : pid_{pid},
      user_{LinuxParser::User(pid)},
      command_{LinuxParser::Command(pid)},
      cpuUtilization_{LinuxParser::CpuUtilization(pid)},
      ram_{LinuxParser::Ram(pid)},
      uptime_{LinuxParser::UpTime(pid)} {}

int Process::Pid() const { return pid_; }

float Process::CpuUtilization() const { return cpuUtilization_; }

string Process::Command() const { return command_; }

string Process::Ram() const { return ram_; }

string Process::User() const { return user_; }

long int Process::UpTime() const { return uptime_; }

bool Process::operator<(Process const& a) const { return cpuUtilization_ > a.cpuUtilization_; }