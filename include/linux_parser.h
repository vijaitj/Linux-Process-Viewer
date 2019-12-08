#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// Memory Categories (for easy look up and and match)
enum MemoryCategories {
  kMemUnknownCategory_,
  kMemTotal_,
  kMemFree_,
  kMemBuffer_,
  kMemCached_,
  kMemShmem_,
  kMemSreclaimable_
};

// 
static std::unordered_map<std::string, MemoryCategories> mapMemCategories;
void EstablishMemStringMap();
// Helper to clear string streams
void ClearStringStream(std::stringstream&);

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};

// Processor
enum ProcesorStates {
  kUTime_ = 13,
  kSTime_ = 14,
  kCUTime_= 15,
  kCSTime_ = 16,
  kStartTime_ = 21
};

// Functions to compute CPU utilization
std::pair<long,long> Jiffies();
std::pair<long,long> Jiffies(int pid, long uptime);
// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);
};  // namespace LinuxParser

#endif
