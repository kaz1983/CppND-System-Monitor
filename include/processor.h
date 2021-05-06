#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  float total_{0};
  float idle_{0};
  float totald_{0};
  float idled_{0};
  float previousIdel_{0};
  float previousTotal_{0};
  float aggregateCpuUtilization_{0};
};

#endif