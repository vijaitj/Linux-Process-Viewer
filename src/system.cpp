#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

//Constructor
System::System() {
    // call the function to set up the hastable for memory categories. We have to do it just once.
    LinuxParser::EstablishMemStringMap();
    // the below member variables can be set just once and reused later (won't be modified in between)
    System::kernel_ = LinuxParser::Kernel();
    System::operating_system_ = LinuxParser::OperatingSystem();
  };

// Returns the system's CPU
Processor& System::Cpu() { 
  return cpu_; 
}

// Returns a container composed of the system's processes
vector<Process>& System::Processes() {
  //get process Ids first
  vector<int> pids = LinuxParser::Pids();
  //Clear the existing vector of processes
  System::processes_.clear();
  // For each ID create a process and add it to the vector
  for(auto pid : pids){
    auto p = Process(pid,System::UpTime());
    System::processes_.push_back(p);
  }
  // Sort based on RAM usage  for better display.
  std::sort(System::processes_.begin(),System::processes_.end(),std::greater<>());
  return System::processes_;
}

// Returns the system's kernel identifier (string)
std::string System::Kernel() {
  return System::kernel_;
}

// Returns the system's memory utilization
float System::MemoryUtilization() {
  return LinuxParser::MemoryUtilization();
}

// Returns the operating system name
std::string System::OperatingSystem() {
  return System::operating_system_;
}

// Returns the number of processes actively running on the system
int System::RunningProcesses() {
  return LinuxParser::RunningProcesses();
}

// Returns the total number of processes on the system
int System::TotalProcesses() {
  return LinuxParser::TotalProcesses();
}

// Returns the number of seconds since the system started running
long int System::UpTime() {
  return LinuxParser::UpTime();
}