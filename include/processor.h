#ifndef PROCESSOR_H
#define PROCESSOR_H


class Processor {
 public:
  // Constructor
  Processor() {
    active_ = 0;
    idle_ = 0;
    percent_utilization_ = 0;
  }
  //Getters
  long Active() const{
    return active_;
  }
  long Idle() const{
    return idle_;
  }
  //Setters
  void Active(long val){
    active_ = val;
  }
  void Idle(long val){
    idle_ = val;
  }
  float Utilization();

 private:
  // Stores latest idle time and active time and percentage utilization.
  // The active and idle time are stored to facilitate the calculation 
  // of current utilization as supposed to all time utilization. 
  long active_;
  long idle_;
  float percent_utilization_;
};

#endif