#include "processor.h"
#include "linux_parser.h"

// Returns the aggregate CPU utilization
float Processor::Utilization() { 
    // Call Jiffies to get current values
    std::vector<long> cur_jiffies = LinuxParser::Jiffies();
    //Compute differences (to get current utilization instead of all time utilization)
    long dif_tot = cur_jiffies[0] + cur_jiffies[1] - (Processor::Active() + Processor::Idle());
    long dif_idle = cur_jiffies[1] - Processor::Idle();
    // Update the active and idle values for jiffies 
    // (to be used as previous values next time)
    Processor::Active(cur_jiffies[0]);
    Processor::Idle(cur_jiffies[1]);
    // Compute and return the current utilization
    Processor::percent_utilization_ = (float)(dif_tot - dif_idle)/dif_tot;
    return Processor::percent_utilization_;
}