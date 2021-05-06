#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
 public:
  Process(int pid);
  int Pid() const;
  std::string User() const;
  std::string Command() const;
  float CpuUtilization() const;
  std::string Ram() const;
  long int UpTime() const;
  bool operator<(Process const& a) const;

 private:
  int pid_{0};
  std::string user_{""};
  std::string command_{""};
  float cpuUtilization_{0.0};
  std::string ram_{""};
  long int uptime_{0};
};

#endif