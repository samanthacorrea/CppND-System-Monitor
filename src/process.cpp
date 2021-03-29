#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {}

int Process::Pid() const { 
  return this->pid_; 
}

float Process::CpuUtilization() const { 
  return this->cpu_; 
}

void Process::previousUse(long activated, long systick) {
  long activated_timer{activated - this->activated_};
  long systick_timer{systick - this->systick_};
  this->cpu_ = static_cast<float>(activated_timer) / systick_timer;
  this->activated_ = activated;
  this->systick_ = systick;
}

string Process::Command() const { 
  return LinuxParser::Command(this->pid_); 
}

string Process::Ram() const { 
  return LinuxParser::Ram(this->pid_); 
}

string Process::User() const { 
  return LinuxParser::User(this->pid_); 
}

long int Process::UpTime() const { 
  return LinuxParser::UpTime(this->pid_);
}

bool Process::operator>(const Process& a) const {
  return this->CpuUtilization() > a.CpuUtilization();
}

bool Process::operator<(const Process& a) const {
  return this->CpuUtilization() < a.CpuUtilization();
}