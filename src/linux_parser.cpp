#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line{""};
  string key{""};
  string value{""};
  float totalUsableMemory{0.0};
  float totalFreeMemory{0.0};

  std::ifstream memoryInfoStream(kProcDirectory + kMeminfoFilename);
  if (memoryInfoStream.is_open()) {
    while (std::getline(memoryInfoStream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key.compare("MemTotal:") == 0) {
          totalUsableMemory = stof(value);
        } else if (key.compare("MemFree:") == 0) {
          totalFreeMemory = stof(value);
        }
      }
    }
  }
  return (totalUsableMemory - totalFreeMemory) / totalUsableMemory;
}

long LinuxParser::UpTime() {
  string line{""};
  string uptime{""};
  string idleTime{""};

  std::ifstream upTimeFileStream{kProcDirectory + kUptimeFilename};
  if (upTimeFileStream.is_open()) {
    std::getline(upTimeFileStream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idleTime;
  }
  return stol(uptime);
}

long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

long LinuxParser::ActiveJiffies(int pid) {
  string line{""};
  string value{""};
  long totalTime{0};

  std::ifstream statFileStream(kProcDirectory + to_string(pid) + kStatFilename);
  if (statFileStream.is_open()) {
    std::getline(statFileStream, line);
    std::replace(line.begin(), line.end(), '(', ' ');
    std::replace(line.begin(), line.end(), ')', ' ');
    std::istringstream linestream(line);
    std::istream_iterator<string> begin(linestream), end;
    vector<string> data(begin, end);
    totalTime = stol(data.at(kUtime_ - 1)) + stol(data.at(kStime_ - 1)) +
                stol(data.at(kCutime_ - 1)) + stol(data.at(kCstime_ - 1));
  }
  return totalTime;
}

long LinuxParser::ActiveJiffies() {
  string line{""};
  string key{""};
  string value{""};

  vector<long> jiffies{};

  std::ifstream statFileStream(kProcDirectory + kStatFilename);
  if (statFileStream.is_open()) {
    std::getline(statFileStream, line);
    std::istringstream linestream(line);
    linestream >> key;
    if (key.compare("cpu") == 0) {
      while (linestream >> value) {
        jiffies.emplace_back(stol(value));
      }
    }
  }
  return (jiffies.at(kUser_) + jiffies.at(kNice_) + jiffies.at(kSystem_) +
          jiffies.at(kIRQ_) + jiffies.at(kSoftIRQ_) + jiffies.at(kSteal_));
}

long LinuxParser::IdleJiffies() {
  string line{""};
  string key{""};
  string value{""};
  vector<long> jiffies{};

  std::ifstream statFileStream(kProcDirectory + kStatFilename);
  if (statFileStream.is_open()) {
    std::getline(statFileStream, line);
    std::istringstream linestream(line);
    linestream >> key;
    if (key.compare("cpu") == 0) {
      while (linestream >> value) {
        jiffies.emplace_back(stol(value));
      }
    }
  }
  return (jiffies.at(kIdle_) + jiffies.at(kIOwait_));
}

std::vector<std::string> LinuxParser::CpuUtilization() {
  std::vector<string> cpuUtilization{};
  cpuUtilization.emplace_back(to_string(Jiffies()));
  cpuUtilization.emplace_back(to_string(IdleJiffies()));
  return cpuUtilization;
}

float LinuxParser::CpuUtilization(int pid) {
  string line{""};
  string value{""};
  float startTime{0};
  float totalTime{0};
  float seconds{0};
  const float clockTicks = sysconf(_SC_CLK_TCK);

  std::ifstream statFileStream(kProcDirectory + to_string(pid) + kStatFilename);
  if (statFileStream.is_open()) {
    std::getline(statFileStream, line);
    std::replace(line.begin(), line.end(), '(', ' ');
    std::replace(line.begin(), line.end(), ')', ' ');
    std::istringstream linestream(line);
    std::istream_iterator<string> begin(linestream), end;
    vector<string> data(begin, end);
    startTime = stof(data.at(kStarttime_ - 1)) / clockTicks;
  }
  totalTime = (float)(ActiveJiffies(pid)) / clockTicks;
  seconds = (float)UpTime(pid) - startTime;

  return 1.0 * (totalTime / seconds);
}

int LinuxParser::TotalProcesses() {
  string line{""};
  string key{""};
  string value{""};
  int totalProcesses{0};

  std::ifstream statFileStream(kProcDirectory + kStatFilename);
  if (statFileStream.is_open()) {
    while (std::getline(statFileStream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key.compare("processes") == 0 && value != "")
        totalProcesses = stoi(value);
    }
  }

  return totalProcesses;
}

int LinuxParser::RunningProcesses() {
  string line{""};
  string key{""};
  string value{""};
  int numberOfRunningProcess{0};

  std::ifstream statFileStream(kProcDirectory + kStatFilename);
  if (statFileStream.is_open()) {
    while (std::getline(statFileStream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key.compare("procs_running") == 0 && value != "")
        numberOfRunningProcess = stoi(value);
    }
  }
  return numberOfRunningProcess;
}

string LinuxParser::Command(int pid) {
  string line{""};

  std::ifstream commandLineStream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (commandLineStream.is_open()) {
    std::getline(commandLineStream, line);
  }

  return line;
}

string LinuxParser::Ram(int pid) {
  string line{""};
  string key{""};
  string value{""};
  long ram{0};

  std::ifstream statusFileStream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (statusFileStream.is_open()) {
    while (std::getline(statusFileStream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key.compare("VmSize:") == 0) ram = stol(value);
    }
  }

  return std::to_string(ram / 1000);
}

string LinuxParser::Uid(int pid) {
  string line{""};
  string key{""};
  string value{""};
  string userID{""};

  std::ifstream statusFileStream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (statusFileStream.is_open()) {
    while (std::getline(statusFileStream, line)) {
      std::replace(line.begin(), line.end(), '\t', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key.compare("Uid:") == 0) userID = value;
    }
  }

  return userID;
}

string LinuxParser::User(int pid) {
  string line{""};
  string userName{""};
  string x{""};
  string userID{""};

  std::ifstream passwordPathStream(kPasswordPath);
  if (passwordPathStream.is_open()) {
    while (std::getline(passwordPathStream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> userName >> x >> userID;
      if (userID.compare(Uid(pid)) == 0) break;
    }
  }

  return userName;
}

long LinuxParser::UpTime(int pid) {
  string line{""};
  long startTime{0};

  std::ifstream statFileStream(kProcDirectory + to_string(pid) + kStatFilename);
  if (statFileStream.is_open()) {
    std::getline(statFileStream, line);
    std::replace(line.begin(), line.end(), '(', ' ');
    std::replace(line.begin(), line.end(), ')', ' ');
    std::istringstream linestream(line);
    std::istream_iterator<string> begin(linestream), end;
    vector<string> data(begin, end);
    startTime = stol(data.at(kStarttime_ - 1)) / sysconf(_SC_CLK_TCK);
  }

  return UpTime() - startTime;
}
