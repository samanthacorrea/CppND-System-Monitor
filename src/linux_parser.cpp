#include <dirent.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "linux_parser.h"

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
  string os, hostname, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> hostname >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    if (file->d_type == DT_DIR) {
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
  vector<float> memory_vector;
  string line, row;
  float value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> row >> value) {
        if (row == "MemTotal:") {
          memory_vector.push_back(value);
        }
        if (row == "MemFree:") {
          memory_vector.push_back(value);
        }
      }
    }
  }

  return (memory_vector[0] - memory_vector[1]) / memory_vector[0];
}

long LinuxParser::UpTime() { //here
  string line;
  long uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}


long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  vector<string> values;
  long totalTime;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                           LinuxParser::kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  
  if (values.size() > 21) {
    long userTime = stol(values[13]);
    long sysTime = stol(values[14]);
    long cUserTime = stol(values[15]);
    long cSysTime = stol(values[16]);
    totalTime = userTime + sysTime + cUserTime + cSysTime;
  }

  return totalTime;
}

long LinuxParser::ActiveJiffies() {
  vector<string> jiffies = CpuUtilization();
  return (stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) +
          stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) +
          stol(jiffies[CPUStates::kSoftIRQ_]) + stol(jiffies[CPUStates::kSteal_]) +
          stol(jiffies[CPUStates::kGuest_]) + stol(jiffies[CPUStates::kGuestNice_]));
}

long LinuxParser::IdleJiffies() {
  vector<string> jiffies = CpuUtilization();
  return (stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]));
}

vector<string> LinuxParser::CpuUtilization() {
  string line, value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value) {
        if (value == "cpu") {
          while (linestream >> value) values.push_back(value);
          return values;
        }
      }
    }
  }

  return values;
}


int LinuxParser::TotalProcesses() {
  string line, key;
  int total_processes;
 
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "processes") {
        linestream >> total_processes;
      	return total_processes;
      }
    }
  }

  return total_processes;
}

int LinuxParser::RunningProcesses() {
  string line, key, val;
  int running_processes;
  std::ifstream stream(kProcDirectory + kStatFilename);
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "procs_running") {
        linestream >> running_processes;
        return running_processes;
      }
    }
  }
	
  return running_processes;
}

string LinuxParser::Command(int pid) {
  string command{""};
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                       LinuxParser::kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, command);
  }

  return command;
}

string LinuxParser::Ram(int pid) {
  string key;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                       LinuxParser::kStatusFilename);
  if (stream.is_open()) {
    while (stream >> key) {
      if (key == "VmSize:") {
        if (stream >> key) 
          return std::to_string(stoi(key) / 1024);
      }
    }
  }

  return key;
}

string LinuxParser::Uid(int pid) {
  string uid;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                       LinuxParser::kStatusFilename);
  if (stream.is_open()) {
    while (stream >> uid) {
      if (uid == "Uid:") {
        if (stream >> uid) 
          return uid;
      }
    }
  }
  return uid;
}

string LinuxParser::User(int pid) {
  string user_number = LinuxParser::Uid(pid);
  string line, user_name, uid;
  std::ifstream stream(LinuxParser::kPasswordPath);
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), 'x', ' ');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user_name >> uid;
      if (uid == user_number) {
          return user_name;
      
      }
    }
  }
  return user_name;
}

long LinuxParser::UpTime(int pid) {
  int uptime;
  string line, value;
  vector<string> values;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                       LinuxParser::kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  uptime = UpTime() - stol(values[21])/sysconf(_SC_CLK_TCK);
  return uptime;
}