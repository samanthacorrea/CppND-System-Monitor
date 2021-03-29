#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization() {
  float utilization{0};
  long activated = LinuxParser::ActiveJiffies();
  long otiose = LinuxParser::IdleJiffies();
  long activated_timer{activated - activated_};
  long otiose_timer{otiose - otiose_};
  long timer{activated_timer + otiose_timer};
  utilization = static_cast<float>(activated_timer) / timer;
  activated_ = activated;
  otiose_ = otiose;

  return utilization;
}