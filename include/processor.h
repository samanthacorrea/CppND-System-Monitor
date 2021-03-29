#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();
  
 private:
  long activated_{0};
  long otiose_{0};
};

#endif