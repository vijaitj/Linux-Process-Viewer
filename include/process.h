#ifndef PROCESS_H
#define PROCESS_H

#include <string>

/*
Basic class for Process representation
*/
class Process {
 public:
  Process(int id, long sysUptime);
  int Pid();                               
  std::string User();
  std::string Uid();                     
  std::string Command();                   
  float CpuUtilization();                  
  std::string Ram();                       
  long int UpTime();
  void UpdateCpuUtilization();
  void Update();
  bool operator<(Process const& a) const;
  bool operator>(Process const& a) const;
 private:
  int pid_;
  std::string uid_;
  long int sysUpTime_;
  long tot_spent_;
  long tot_elapsed_;
  float cpuUtilization_;
  int ram_;
  long int upTime_;
  std::string user_;
  std::string command_;
};

#endif