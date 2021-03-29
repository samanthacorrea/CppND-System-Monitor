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
  void previousUse(long activated, long systick);
  std::string Ram() const;                 
  long int UpTime() const;                 
  bool operator<(Process const& a) const;  
  bool operator>(Process const& a) const;  

 private:
  int pid_{0};
  float cpu_{0};
  long activated_{0};
  long otiose_{0};
  long systick_{0};
};

#endif
