#include "generic_robot/subsystem_runner/subsystem_runner.h"

namespace generic_robot {

void SubsystemRunner::operator()() {
  aos::time::PhasedLoop phased_loop(std::chrono::milliseconds(5));

  // TODO(Kyle or Wesley) Come up with some actual value for this...
  aos::SetCurrentThreadRealtimePriority(10);
  aos::SetCurrentThreadName("SubsystemRunner");

  running_ = true;

  while (running_) {
    wpilib_.ReadSensors();
    // Update subsystems here

    wpilib_.WriteActuators();

    phased_loop.SleepUntilNext();
  }
}

void SubsystemRunner::Stop() { running_ = false; }

}  // namespace generic_robot
