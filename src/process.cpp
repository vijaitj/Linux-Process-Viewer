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

// Constructor
  Process::Process(int id, long sysUptime){
      pid_ = id;
      sysUpTime_ = sysUptime;
      Update();
      tot_elapsed_ = 0;
      tot_spent_ = 0;
  }

// Returns this process's ID
int Process::Pid() { 
    return pid_; 
    }

// Returns the process user's ID
std::string Process::Uid(){
    return uid_;
}

// Returns this process's CPU utilization
float Process::CpuUtilization() {
    return cpuUtilization_;
}

// Returns the command that generated this process
string Process::Command() { 
    return command_;
 }

// Returns this process's memory utilization
string Process::Ram() { 
    return std::to_string(ram_);
}

// Returns the user (name) that generated this process
string Process::User() {
    return user_;
}

// Returns the age of this process (in seconds)
long int Process::UpTime() {
  	return upTime_;
 }

// Update CPU Utilization
void Process::UpdateCpuUtilization(){
      // Call Jiffies to get current values
    std::pair<long,long> cur_jiffies = LinuxParser::Jiffies(pid_,sysUpTime_);
    // find the difference in spent and elapsed times w.r.t previous values
    // this wont help as of now as we are clearing the vector of processes every time.
    // Todo: Update processes instead of clearing them. 
    // The tricky part is deciding whether to update the process or delete it (delete if old process died)
    long dif_spent = cur_jiffies.first - tot_spent_;
    long dif_elapsed = cur_jiffies.second - tot_elapsed_;
    // Save the currently obtained values for using in calcultions next time
    tot_spent_ = cur_jiffies.first;
    tot_elapsed_ = cur_jiffies.second;
    // Compute percentage, store and return
    cpuUtilization_ = ((float)dif_spent/dif_elapsed);
}

// Used to update the member variables.
void Process::Update(){
  	// Command
  	command_ = LinuxParser::Command(pid_);
    // Uptime
    upTime_ =  LinuxParser::UpTime(pid_);
  	// User ID
  	uid_ = LinuxParser::Uid(pid_);
    //User
    std::stringstream sstrm(uid_);
    int tmp;
    sstrm >> tmp;
    user_ = LinuxParser::User(tmp);
  	// RAM
  	std::string rm =  LinuxParser::Ram(pid_);
    std::stringstream sstrm2(rm);
    sstrm2 >> ram_;
  	// CPU Utilization
  	UpdateCpuUtilization();
}
// Overloads the "less than" comparison operator for Process objects.
// Facilitates sort w.r.t ram use of processes.
bool Process::operator<(Process const& a) const { 
     return ram_ < a.ram_ ;
}
bool Process::operator>(Process const& a) const { 
    return ram_ > a.ram_;
}