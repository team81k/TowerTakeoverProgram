#include "vex.h"
#include "variables.h"

void handleSensorValues()
{
  //drive
  drive.rightPosition = FR.position(vex::deg) * 0.5 + BR.position(vex::deg) * 0.5;
  drive.leftPosition = FL.position(vex::deg) * 0.5 + BL.position(vex::deg) * 0.5;

  //tilter
  tilter.position = (FR.position(vex::deg) - BR.position(vex::deg)) + tilter.offset;
  tilter.angle = 45.0 - tilter.position * (45.0 / 1200.0);

  //arm
  arm.position = ArmL.position(vex::deg) + arm.offset;
  arm.angle = 180.0 - std::fmax((arm.position - 340.0) / (84.0 / 12.0), 0.0);
  arm.absoluteAngle = arm.angle - tilter.angle;
  //always set offset
  if(ArmPot.value(vex::rotationUnits::raw) < 4000 && ArmPot.value(vex::rotationUnits::raw) > 100)
  {
    arm.offset -= arm.position
      - (162 - ArmPot.value(vex::rotationUnits::deg)) //pot offset
      * (84.0 / 12.0); //gearing
  }

  //intakes
  intake.avgPosition = IntakeR.position(vex::deg) * 0.5 + IntakeL.position(vex::deg) * 0.5;
}

void applyMotorPower()
{
  //differential
  FR.spin(vex::forward, drive.rightPower + tilter.power, vex::percent);
  FL.spin(vex::forward, drive.leftPower + tilter.power, vex::percent);
  BR.spin(vex::forward, drive.rightPower - tilter.power, vex::percent);
  BL.spin(vex::forward, drive.leftPower - tilter.power, vex::percent);

  //Arm
  ArmR.setBrake(vex::brakeType::brake);
  ArmL.setBrake(vex::brakeType::brake);
  //ArmR.setMaxTorque(, percentUnits units)
  ArmR.spin(vex::forward, arm.power, vex::percent);
  ArmL.spin(vex::forward, arm.power, vex::percent);

  //Intakes
  IntakeR.setBrake(vex::brakeType::brake);
  IntakeL.setBrake(vex::brakeType::brake);
  IntakeR.spin(vex::forward, intake.power, vex::percent);
  IntakeL.spin(vex::forward, intake.power, vex::percent);
}

void handleTimed()
{
  while(true)
  {
    int elapsedTime = vex::timer::system() - preTime;
    double perSec = 1000.0 / elapsedTime;
    double forSec = elapsedTime / 1000.0;
    preTime = vexSystemTimeGet();

    //drive
    drive.rightSpeed = (drive.rightPosition - drive.rightPrePosition) * perSec;
    drive.rightPrePosition = drive.rightPosition;

    drive.leftSpeed = (drive.leftPosition - drive.leftPrePosition) * perSec;
    drive.leftPrePosition = drive.leftPosition;

    if(drive.slew)
    {
      double rightError = drive.preSlewRightPower - drive.rightPower;
      double leftError = drive.preSlewLeftPower - drive.leftPower;
      if(rightError > drive.slewRate * forSec) rightError = drive.slewRate * forSec;
      if(rightError < -drive.slewRate * forSec) rightError = -drive.slewRate * forSec;
      if(leftError > drive.slewRate * forSec) leftError = drive.slewRate * forSec;
      if(leftError < -drive.slewRate * forSec) leftError = -drive.slewRate * forSec;
      drive.rightPower += rightError;
      drive.leftPower += leftError;
    }

    //tilter
    tilter.speed = (tilter.position - tilter.prePosition) * perSec;
    tilter.prePosition = tilter.position;

    if(tilter.slew)
    {
      double error = tilter.preSlewPower - tilter.power;
      if(error > tilter.slewRate * forSec) error = tilter.slewRate * forSec;
      if(error < -tilter.slewRate * forSec) error = -tilter.slewRate * forSec;
      tilter.power += error;
    }

    //arm
    arm.speed = (arm.position - arm.prePosition) * perSec;
    arm.prePosition = arm.position;

    //intakes
    intake.avgSpeed = (intake.avgPosition - intake.avgPrePosition) * perSec;
    intake.avgPrePosition = intake.avgPosition;

    vex::task::sleep(50);
  } 
}