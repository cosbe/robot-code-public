#include "o2016/teleop/teleop.h"
#include "WPILib.h"
#include "muan/wpilib/queue_types.h"
#include "o2016/queue_manager/queue_manager.h"

namespace o2016 {

namespace teleop {

Teleop::Teleop() : throttle_{1}, wheel_{0}, gamepad_{2} {
  shifting_low_ = throttle_.MakeButton(4);
  shifting_high_ = throttle_.MakeButton(5);
  quickturn_ = throttle_.MakeButton(5);

  intake_ = gamepad_.MakeButton(6);
  outtake_ = gamepad_.MakeButton(4);
  tucknroll_ = throttle_.MakeButton(2);
  thisisadorable_ = gamepad_.MakeButton(2);
  settledown_ = gamepad_.MakeButton(5);
  snap_ = gamepad_.MakeButton(3);

  Update();
}

void Teleop::Update() {
  // Update joysticks
  throttle_.Update();
  wheel_.Update();
  gamepad_.Update();

  SendDSMessage();
  SendDrivetrainMessage();
  SendTurretMessage();
  SendIntakeMessage();
}

void Teleop::SendDSMessage() {
  muan::wpilib::DriverStationProto status;

  if (DriverStation::GetInstance().IsDisabled()) {
    status->set_mode(RobotMode::DISABLED);
  } else if (DriverStation::GetInstance().IsAutonomous()) {
    status->set_mode(RobotMode::AUTONOMOUS);
  } else if (DriverStation::GetInstance().IsOperatorControl()) {
    status->set_mode(RobotMode::TELEOP);
  } else {
    status->set_mode(RobotMode::ESTOP);
  }

  status->set_battery_voltage(DriverStation::GetInstance().GetBatteryVoltage());
  status->set_brownout(DriverStation::GetInstance().IsBrownedOut());
  status->set_has_ds_connection(DriverStation::GetInstance().IsDSAttached());

  o2016::QueueManager::GetInstance().driver_station_queue().WriteMessage(
      status);
}

void Teleop::SendDrivetrainMessage() {
  o2016::drivetrain::DrivetrainGoalProto drivetrain_goal;

  double throttle = -throttle_.wpilib_joystick()->GetRawAxis(1);
  double wheel = -wheel_.wpilib_joystick()->GetRawAxis(0);
  bool quickturn = quickturn_->is_pressed();

  double angular;
  double forward = throttle;

  if (shifting_high_->was_clicked()) {
    high_gear_ = true;
  } else if (shifting_low_->was_clicked()) {
    high_gear_ = false;
  }

  if (quickturn) {
    angular = (high_gear_ ? 5 : 3) * wheel;
  } else {
    angular = (high_gear_ ? 5 : 3) * throttle * wheel;
  }

  drivetrain_goal->mutable_velocity_command()->set_forward_velocity(forward);
  drivetrain_goal->mutable_velocity_command()->set_angular_velocity(angular);
  drivetrain_goal->set_gear(high_gear_ ? o2016::drivetrain::Gear::kHighGear
                                       : o2016::drivetrain::Gear::kLowGear);

  o2016::QueueManager::GetInstance().drivetrain_goal_queue().WriteMessage(
      drivetrain_goal);
}

void Teleop::SendTurretMessage() {
  if (snap_->was_clicked() || intake_->is_pressed() ||
      thisisadorable_->was_clicked()) {
    o2016::turret::TurretGoalProto goal;
    goal->set_goal_angle(0);
    o2016::QueueManager::GetInstance().turret_goal_queue().WriteMessage(goal);
  }
}

void Teleop::SendIntakeMessage() {
  if (intake_->is_pressed()) {
    o2016::intake::IntakeGoalProto goal;
    goal->set_goal_angle(0);
    goal->set_intake_speed(o2016::intake::RollerGoal::FORWARD);
    o2016::QueueManager::GetInstance().intake_goal_queue().WriteMessage(goal);
  } else if (intake_->was_released()) {
    o2016::intake::IntakeGoalProto goal;
    goal->set_goal_angle(0);
    goal->set_intake_speed(o2016::intake::RollerGoal::STOP);
    o2016::QueueManager::GetInstance().intake_goal_queue().WriteMessage(goal);

  } else if (tucknroll_->was_clicked()) {
    o2016::intake::IntakeGoalProto goal;
    goal->set_goal_angle(-.2);
    goal->set_intake_speed(o2016::intake::RollerGoal::STOP);
    o2016::QueueManager::GetInstance().intake_goal_queue().WriteMessage(goal);
  } else if (thisisadorable_->was_clicked()) {
    o2016::intake::IntakeGoalProto goal;
    goal->set_goal_angle(.8);
    goal->set_intake_speed(o2016::intake::RollerGoal::STOP);
    o2016::QueueManager::GetInstance().intake_goal_queue().WriteMessage(goal);
  } else if (settledown_->is_pressed()) {
  } else {
  }
}

}  // teleop

}  // o2016