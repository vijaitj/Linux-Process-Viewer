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
      Process::pid_ = id;
      Process::sysUpTime_ = sysUptime;
      Process::Update();
      Process::tot_elapsed_ = 0;
      Process::tot_spent_ = 0;
  }

// Returns this process's ID
int Process::Pid() { 
    return Process::pid_; 
    }

// Returns the process user's ID
std::string Process::Uid(){
    return Process::uid_;
}

// Returns this process's CPU utilization
float Process::CpuUtilization() {
    return Process::cpuUtilization_;
}

// Returns the command that generated this process
string Process::Command() { 
    return Process::command_;
 }

// Returns this process's memory utilization
string Process::Ram() { 
    return std::to_string(Process::ram_);
}

// Returns the user (name) that generated this process
string Process::User() {
    return Process::user_;
}

// Returns the age of this process (in seconds)
long int Process::UpTime() {
  	return Process::upTime_;
 }

// Update CPU Utilization
void Process::UpdateCpuUtilization(){
      // Call Jiffies to get current values
    std::vector<long> cur_jiffies = LinuxParser::Jiffies(Process::pid_,Process::sysUpTime_);
    // find the difference in spent and elapsed times w.r.t previous values
    // this wont help as of now as we are clearing the vector of processes every time.
    // Todo: Update processes instead of clearing them. 
    // The tricky part is deciding whether to update the process or delete it (delete if old process died)
    long dif_spent = cur_jiffies[0] - Process::tot_spent_;
    long dif_elapsed = cur_jiffies[1] - Process::tot_elapsed_;
    // Save the currently obtained values for using in calcultions next time
    Process::tot_spent_ = cur_jiffies[0];
    Process::tot_elapsed_ = cur_jiffies[1];
    // Compute percentage, store and return
    Process::cpuUtilization_ = ((float)dif_spent/dif_elapsed);
}

// Used to update the member variables.
void Process::Update(){
  	// Command
  	Process::command_ = LinuxParser::Command(Process::pid_);
    // Uptime
    Process::upTime_ =  LinuxParser::UpTime(Process::pid_);
  	// User ID
  	Process::uid_ = LinuxParser::Uid(Process::pid_);
    //User
    std::stringstream sstrm(Process::uid_);
    int tmp;
    sstrm >> tmp;
    Process::user_ = LinuxParser::User(tmp);
  	// RAM
  	std::string rm =  LinuxParser::Ram(Process::pid_);
    std::stringstream sstrm2(rm);
    sstrm2 >> Process::ram_;
  	// CPU Utilization
  	Process::UpdateCpuUtilization();
}
// Overloads the "less than" comparison operator for Process objects.
// Facilitates sort w.r.t ram use of processes.
bool Process::operator<(Process const& a) const { 
    if(Process::ram_ < a.ram_){return true;}
    else{ return false;}
}
bool Process::operator>(Process const& a) const { 
    if(Process::ram_ < a.ram_){return false;}
    else{ return true;}
}