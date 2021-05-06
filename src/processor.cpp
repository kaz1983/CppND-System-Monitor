#include "processor.h"

#include <unistd.h>

#include <string>
#include <vector>

#include "linux_parser.h"

float Processor::Utilization() {
  std::vector<std::string> utilData = LinuxParser::CpuUtilization();
  std::vector<float> aggregateCpuUtilData{};

  for (const std::string& data : utilData) {
    aggregateCpuUtilData.emplace_back(stof(data));
  }

  total_ = aggregateCpuUtilData[0];
  idle_ = aggregateCpuUtilData[1];

  totald_ = total_ - previousTotal_;
  idled_ = idle_ - previousIdel_;
  if (totald_ != 0) aggregateCpuUtilization_ = (totald_ - idled_) / totald_;

  previousTotal_ = aggregateCpuUtilData[0];
  previousIdel_ = aggregateCpuUtilData[1];

  return aggregateCpuUtilization_;
}