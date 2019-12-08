#include <dirent.h>
#include <unistd.h>
#include <map>
#include <string>
#include <vector>
#include <sstream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Returns the name of the OS
string LinuxParser::OperatingSystem() {
  string key; 
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    string line;
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

// Returns the name of the Kernel
string LinuxParser::Kernel() {
  string os; 
  string kernel; 
  string version;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// Returns the Ids of the processes by reading directory names.
// Todo: Update this to use std::filesystem
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

// Establish the map between string version of memory categeries and their enum
// types
void LinuxParser::EstablishMemStringMap() {
  mapMemCategories["MemTotal:"] = kMemTotal_;
  mapMemCategories["MemFree:"] = kMemFree_;
  mapMemCategories["Buffers:"] = kMemBuffer_;
  mapMemCategories["Cached:"] = kMemCached_;
  mapMemCategories["Shmem:"] = kMemShmem_;
  mapMemCategories["SReclaimable:"] = kMemSreclaimable_;
}

// Clear a string stream
void LinuxParser::ClearStringStream(std::stringstream& sstrm) {
  sstrm.str(std::string());
  sstrm.clear();
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  string line;
  string key;
  string value;
  std::stringstream sstrm;
  int mem_total, mem_free, mem_buffer, mem_cahed, mem_shmem, mem_sreclaimable;
  if(filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        // Any unknown memory category would be added to hashtable if not found
        // in it, we need to avoid this. Therefore, if no mathch for the key
        // exists, skip.
        if (LinuxParser::mapMemCategories.find(key) == LinuxParser::mapMemCategories.end()) {
          continue;
        }
        switch (LinuxParser::mapMemCategories[key]) {
          case kMemTotal_:
            LinuxParser::ClearStringStream(sstrm);
            sstrm.str(value);
            sstrm >> mem_total;
            break;
          case kMemFree_:
            LinuxParser::ClearStringStream(sstrm);
            sstrm.str(value);
            sstrm >> mem_free;
            break;
          case kMemBuffer_:
            LinuxParser::ClearStringStream(sstrm);
            sstrm.str(value);
            sstrm >> mem_buffer;
            break;
          case kMemCached_:
            LinuxParser::ClearStringStream(sstrm);
            sstrm.str(value);
            sstrm >> mem_cahed;
            break;
          case kMemShmem_:
            LinuxParser::ClearStringStream(sstrm);
            sstrm.str(value);
            sstrm >> mem_shmem;
            break;
          case kMemSreclaimable_:
            LinuxParser::ClearStringStream(sstrm);
            sstrm.str(value);
            sstrm >> mem_sreclaimable;
            break;
          case kMemUnknownCategory_:
            break;  
        }
      }
    }
    return (float)(mem_total - mem_free - mem_shmem + mem_sreclaimable) /
         mem_total;
  }
  return 0.0;
}

// Reads and returns the system uptime
long LinuxParser::UpTime() {
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  std::string line;
  long uptime = 0;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream sstrm(line);
    sstrm >> uptime;
  }
  return uptime;
}

// Read and return the number of jiffies for the system
//long LinuxParser::Jiffies() { return 0; }

// Read and return the number of active jiffies for a PID
//long LinuxParser::ActiveJiffies(int pid) { return 0; }

// Read and return the number of active jiffies for the system
//long LinuxParser::ActiveJiffies() { return 0; }

// Read and return the number of idle jiffies for the system
//long LinuxParser::IdleJiffies() { return 0; }

//Reads and returns CPU utilization
std::pair<long,long> LinuxParser::Jiffies() {
  long jiffies[10] = {0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  std::string line;
  if(filestream.is_open()){
    std::getline(filestream,line);
    std::istringstream sstrm(line);
    std::string ignr;
    sstrm >> ignr;
    int cntr = 0;
    //Read all Jiffies into the array
    while(cntr < 10){
      sstrm >> jiffies[cntr];
      cntr++ ;
    }
  }
  // Compute active and idle jiffies and return them as a vector
  long idle = jiffies[kIdle_] + jiffies[kIOwait_];
  long active = jiffies[kUser_] + jiffies[kNice_] + jiffies[kSystem_] + \
                jiffies[kIRQ_] + jiffies[kSoftIRQ_] + jiffies[kSteal_];
  return  std::pair<long,long>{active,idle};
}

//Read and return CPU utilization for a process
std::pair<long,long> LinuxParser::Jiffies(int pid, long uptime) {
  long jiffies[21] = {0};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  std::string line;
  if(filestream.is_open()){
    std::getline(filestream,line);
    std::istringstream sstrm(line);
    std::string ignr;
    int cntr = 0;
    //Read all relevant Jiffies into the array
    while(cntr < 22){
      if(cntr == 21 || (cntr >= 13 && cntr <= 16)){
        sstrm >> jiffies[cntr];
      }
      else{
        sstrm >> ignr;
      }
      cntr++ ;
    }
  }
  // compute the active and idle times for the process, return them as a vector.
  long tot_spent = jiffies[kUTime_] + jiffies[kSTime_] + jiffies[kCSTime_] + jiffies[kCUTime_];
  // convert from clock ticks to seconds
  tot_spent /= sysconf(_SC_CLK_TCK);
  long tot_elapsed = uptime - (jiffies[kStartTime_]/sysconf(_SC_CLK_TCK));
  return  std::pair<long,long> {tot_spent,tot_elapsed};
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  std::string line, tok1;
  int procs = 0;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream sstrm(line);
      sstrm >> tok1;
      if (tok1 == "processes") {
        sstrm >> procs;
        return procs;
      }
    }
  }
  return procs;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  std::string line, tok1;
  int procs = 0;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream sstrm(line);
      sstrm >> tok1;
      if (tok1 == "procs_running") {
        sstrm >> procs;
        return procs;
      }
    }
  }
  return procs;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { 
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  std::string line, tokn;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream sstrm(line);
    sstrm >> tokn;
  }
  return tokn;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  std::string line, tok1;
  long sz = 0;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream sstrm(line);
      sstrm >> tok1;
      if (tok1 == "VmSize:") {
        sstrm >> sz;
        //Convert to MB
        sz /= 1000;
        return std::to_string(sz);
      }
    }
  }
  return std::string{"0"};
}

// Returns the user ID associated with a process
std::string LinuxParser::Uid(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  std::string line, tok1;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream sstrm(line);
      sstrm >> tok1;
      if (tok1 == "Uid:") {
        sstrm >> tok1;
        return tok1;
      }
    }
  }
  return std::string{};
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::ifstream filestream(kPasswordPath);
  std::string line;
  if (filestream.is_open()) {
    // Go line by line
    while (std::getline(filestream, line)) {
      std::stringstream sstrm(line);
      // Split lines
      std::string tokens[4] ;
      int counter = 0;
      while (std::getline(sstrm, tokens[counter], ':') && counter < 3) {
        counter++;
      }
      // If you found the user return, else continue
      if(std::stoi(tokens[2]) == pid){
        return tokens[0];
      }
    }
  }
  return std::string{};
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  std::string line, tokn;
  long uptime = 0;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream sstrm(line);
    int counter = 0;
    while(counter < 21){
      sstrm >> tokn;
      counter++;
    }
    // Index 21 (item 22) is uptime
    sstrm >> uptime;
    //convert to secs
    uptime /= sysconf(_SC_CLK_TCK);
  }
  return uptime;
}