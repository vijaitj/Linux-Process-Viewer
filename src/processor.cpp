#include "processor.h"
#include "linux_parser.h"

// Returns the aggregate CPU utilization
float Processor::Utilization() { 
    // Call Jiffies to get current values
    std::pair<long,long> cur_jiffies = LinuxParser::Jiffies();
    //Compute differences (to get current utilization instead of all time utilization)
    long dif_tot = cur_jiffies.first + cur_jiffies.second - (Active() + Idle());
    long dif_idle = cur_jiffies.second - Idle();
    // Update the active and idle values for jiffies 
    // (to be used as previous values next time)
    Active(cur_jiffies.first);
    Idle(cur_jiffies.second);
    // Compute and return the current utilization
    percent_utilization_ = static_cast<float>(dif_tot - dif_idle)/dif_tot;
    return percent_utilization_;
}