#include "vex.h"
#include "variables.h"

void logMessage(const char* format, ...)
{
  va_list argptr;
  va_start(argptr, format);

  char cString[255];
  vsnprintf(cString, 255, format, argptr);
  std::string message = cString;
  auton.endMessage += message;
  printf("%s", message.c_str());

  va_end(argptr);
}

void nextAutonomousStep(bool newTime/* = true*/, bool isMacro/* = false*/)
{
  if(isMacro) logMessage("Macro Step %d: Done.\n", macro.step);
  else logMessage("Auton Step %d: Done.\n", auton.step);

  //reset time
  if(newTime) auton.timeStart = vex::timer::system();

  //reset drive
  if(!auton.drivePartWay)
  {
    auton.driveZeroErrorStart = -1;
    auton.driveZeroSpeedStart = -1;
    auton.driveRightStart = drive.rightPosition;
    auton.driveLeftStart = drive.leftPosition;
    auton.driveCannotMove = false;
    auton.driveTimedOut = false;
  }
  auton.drivePartWay = false;

  //reset tilter
  auton.tilterZeroSpeedStart = -1;
  auton.tilterCannotMove = false;
  auton.tilterTimedOut = false;

  //reset arm
  auton.armZeroErrorStart = -1;
  auton.armZeroSpeedStart = -1;
  auton.armCannotMove = false;
  auton.armTimedOut = false;

  //reset macro
  if(!isMacro)
  {
    auton.macroStarted = false;
    auton.macroTimedOut = false;
  }

  //stop drive
  drive.slew = false;
  drive.rightPower = 0;
  drive.leftPower = 0;

  //stop tilter
  tilter.slew = false;
  tilter.preSlewPower = false;
  tilter.power = 0;

  //stop arm
  arm.power = 0;

  if(isMacro) macro.step++;
  else auton.step++;
}

#define clamp(num, min, max) (std::fmin(std::fmax(num, min), max))

bool driveAutonSimple(int dist, bool turn, double kP, double kD, double aKP, double slewRate, int maxSpeed, int threashold, int timeout, double add)
{
  if(auton.driveCannotMove || auton.driveTimedOut)
  {
    drive.slew = false;
    drive.rightPower = 0;
    drive.leftPower = 0;
    return true;
  }

  //pid
  double avgDriveError = 0, avgDriveSpeed = 0, driveSideError = 0, avgDriveErrorAdd = 0;
  double driveRightPower = 0, driveLeftPower = 0;

  if(turn)
  {
    avgDriveError = (dist - (drive.rightPosition - auton.driveRightStart)) * 0.5
      - (-dist - (drive.leftPosition - auton.driveLeftStart)) * 0.5;
    if(avgDriveError < 0) add = -add;
    avgDriveErrorAdd = ((dist + add) - (drive.rightPosition - auton.driveRightStart)) * 0.5
      - (-(dist + add) - (drive.leftPosition - auton.driveLeftStart)) * 0.5;
    avgDriveSpeed = drive.rightSpeed * 0.5 - drive.leftSpeed * 0.5;
    driveSideError = (drive.rightPosition - auton.driveRightStart) + (drive.leftPosition - auton.driveLeftStart);

    double driveStriat = avgDriveErrorAdd * kP - avgDriveSpeed * kD;
    double driveAdjust = driveSideError * aKP;

    driveRightPower = clamp(driveStriat - driveAdjust, -maxSpeed, maxSpeed);
    driveLeftPower = clamp(-driveStriat + driveAdjust, -maxSpeed, maxSpeed);
  }
  else
  {
    avgDriveError = (dist - (drive.rightPosition - auton.driveRightStart)) * 0.5
      + (dist - (drive.leftPosition - auton.driveLeftStart)) * 0.5;
    if(avgDriveError < 0) add = -add;
    avgDriveErrorAdd = ((dist + add) - (drive.rightPosition - auton.driveRightStart)) * 0.5
      + ((dist + add) - (drive.leftPosition - auton.driveLeftStart)) * 0.5;
    avgDriveSpeed = drive.rightSpeed * 0.5 + drive.leftSpeed * 0.5;
    driveSideError = (drive.rightPosition - auton.driveRightStart) - (drive.leftPosition - auton.driveLeftStart);

    double driveStriat = avgDriveErrorAdd * kP - avgDriveSpeed * kD;
    double driveAdjust = driveSideError * aKP;

    driveRightPower = clamp(driveStriat + driveAdjust, -maxSpeed, maxSpeed);
    driveLeftPower = clamp(driveStriat - driveAdjust, -maxSpeed, maxSpeed);
  }

  //apply power (slew rate/no slew)
  if(drive.slewRate == -1)
  {
    drive.slew = false;
    drive.rightPower = driveRightPower;
    drive.leftPower = driveLeftPower;
  }
  else
  {
    drive.slew = true;
    drive.slewRate = slewRate;
    drive.preSlewRightPower = driveRightPower;
    drive.preSlewLeftPower = driveLeftPower;
  }

  //error time
  if(fabs(avgDriveError) <= threashold && auton.driveZeroErrorStart == -1) auton.driveZeroErrorStart = vex::timer::system();
  if(fabs(avgDriveError) > threashold) auton.driveZeroErrorStart = -1;

  //stop time
  if(fabs(drive.rightSpeed) < 50 && fabs(drive.leftSpeed) < 50 && auton.driveZeroSpeedStart == -1) auton.driveZeroSpeedStart = vex::timer::system();
  if(fabs(drive.rightSpeed) > 50 || fabs(drive.leftSpeed) > 50) auton.driveZeroSpeedStart = -1;

  //exit conditions
  bool positionReached = ((vex::timer::system() - auton.driveZeroErrorStart > 250 || useAutonInaccurateDrive) && auton.driveZeroErrorStart != -1),
    driveCannotMove = (vex::timer::system() - auton.driveZeroSpeedStart > 1000 && auton.driveZeroSpeedStart != -1),
    timeoutReached = (vex::timer::system() - auton.timeStart > timeout && timeout != -1);

  //messages and function stop
  if(driveCannotMove)
  {
    logMessage("Step %d: Continued since the drive was not moving.\n", auton.step);
    auton.driveCannotMove = true;
  }
  if(timeoutReached)
  {
    logMessage("Step %d: Drive movment timed out.\n", auton.step);
    auton.driveTimedOut = true;
  }

  return positionReached || driveCannotMove || timeoutReached;
}

bool driveAutonMove(double dist, double maxSpeed/* = 100*/, double threashold/* = 25*/, int timeout/* = -1*/, double add/* = 0*/)
{
  return driveAutonSimple(dist, false, 0.2, 0.0015, /*0.5*/0, 150, maxSpeed, threashold, timeout, add);
}

bool driveAutonTurn(double dist, double maxSpeed/* = 50*/, double threashold/* = 10*/, int timeout/* = -1*/, double add/* = 0*/)
{
  return driveAutonSimple(dist, true, 0.22, 0.01, /*0.02*/0, 150, maxSpeed, threashold, timeout, add);
}

bool driveAutonMovePart(double dist, double maxSpeed, double threashold, double position, int timeout/* = -1*/, double add/* = 0*/)
{
  auton.drivePartWay = true;
  double avgDriveDist = (drive.rightPosition - auton.driveRightStart) * 0.5 + (drive.leftPosition - auton.driveLeftStart) * 0.5;
  bool reachedDist = avgDriveDist > position;
  if(position > dist) reachedDist = avgDriveDist < position;
  return driveAutonMove(dist, maxSpeed, threashold, timeout, add) || reachedDist;
}

bool driveAutonTurnPart(double dist, double maxSpeed, double threashold, double position, int timeout/* = -1*/, double add/* = 0*/)
{
  auton.drivePartWay = true;
  double avgDriveDist = (drive.rightPosition - auton.driveRightStart) * 0.5 - (drive.leftPosition - auton.driveLeftStart) * 0.5;
  bool reachedDist = avgDriveDist > position;
  if(position > dist) reachedDist = avgDriveDist < position;
  return driveAutonTurn(dist, maxSpeed, threashold, timeout, add) || reachedDist;
}

bool tilterAuton(double position, double threashold/* = 50*/, double maxSpeed/* = 100*/, bool fastForward/* = false*/, int cubeCount/* = 10*/, int timeout/* = -1*/)
{
  if(auton.tilterCannotMove || auton.tilterTimedOut)
  {
    tilter.slew = false;
    tilter.power = 0;
    return true;
  }

  double tilterError = (position - tilter.position);

  if(!tilterCalibration())
  {
    tilter.slew = true;
    tilter.slewRate = 100;//75;

    //go forward slow and based on number of cubes
    if(position > 800 && !fastForward)
    {
      double cubeMultiplyer = std::tanh(std::pow(cubeCount, 2) * 0.015);
      double cubeMultiplyer2 = 1.0 / (cubeMultiplyer + 0.5);
      double kP = /*0.048*/0.03 * cubeMultiplyer2, kD = /*0.0065*/0.001 * cubeMultiplyer;
      tilter.preSlewPower = clamp(tilterError * kP - tilter.speed * kD, -maxSpeed, maxSpeed);
    }
    //go fast
    else
    {
      tilter.preSlewPower = clamp(tilterError * 0.06, -maxSpeed, maxSpeed);
    }
  }

  //stop time
  if(fabs(tilter.speed) <= 50 && auton.tilterZeroSpeedStart == -1) auton.tilterZeroSpeedStart = vex::timer::system();
  if(fabs(tilter.speed) > 50) auton.tilterZeroSpeedStart = -1;

  //exit conditions
  bool positionReached = (std::abs(tilterError) < threashold),
    tilterCannotMove = (vex::timer::system() - auton.tilterZeroSpeedStart > 1000 && auton.tilterZeroSpeedStart != -1),
    timeoutReached = (vex::timer::system() - auton.timeStart > timeout && timeout != -1);

  //messages and function stop
  if(tilterCannotMove)
  {
    logMessage("Step %d: Continued since the tilter was not moving.\n", auton.step);
    auton.tilterCannotMove = true;
  }
  if(timeoutReached)
  {
    logMessage("Step %d: Tilter movment timed out.\n", auton.step);
    auton.tilterTimedOut = true;
  }

  return positionReached || tilterCannotMove || timeoutReached;
}

bool tilterAutonPart(double position, double threashold, double endPosition, double maxSpeed/* = 100*/, bool fastForward/* = false*/, int cubeCount/* = 10*/, int timeout/* = -1*/)
{
  bool reachedDist = tilter.position > endPosition;
  if(endPosition > position) reachedDist = tilter.position < endPosition;
  return tilterAuton(position, threashold, maxSpeed, fastForward, cubeCount, timeout) || reachedDist;
}

bool armAuton(double position, double maxSpeed/* = 100*/, double threashold/* = 10*/, bool useHoldPower/* = true*/, int timeout/* = -1*/)
{
  if(auton.armCannotMove || auton.armTimedOut)
  {
    double holdPower = std::fabs(std::sin(arm.absoluteAngle * PI / 180.0)) * 0.6;
    if(arm.position < 360 || !useHoldPower) holdPower = 0;
    arm.power = holdPower;
    return true;
  }

  double armError = (position - arm.position);

  if(!armCalibration())
  {
    double kP = 0.3, kD = 0.02;
    if(position > 360)
    {
      kP = /*0.45*/0.3;
      kD = 0.04;
    }
    arm.power = armError * kP - arm.speed * kD;

    if(arm.power < -50) arm.power = -50;
    arm.power = clamp(arm.power, -maxSpeed, maxSpeed);
  }

  //error time
  if(fabs(armError) <= threashold && auton.armZeroErrorStart == -1) auton.armZeroErrorStart = vex::timer::system();
  if(fabs(armError) > threashold) auton.armZeroErrorStart = -1;

  //stop time
  if(fabs(arm.speed) <= 50 && auton.armZeroSpeedStart == -1) auton.armZeroSpeedStart = vex::timer::system();
  if(fabs(arm.speed) > 50) auton.armZeroSpeedStart = -1;

  //exit conditions
  bool positionReached = (vex::timer::system() - auton.armZeroErrorStart > 250 && auton.armZeroErrorStart != -1),
    armCannotMove = (vex::timer::system() - auton.armZeroSpeedStart > 1000 && auton.armZeroSpeedStart != -1),
    timeoutReached = (vex::timer::system() - auton.timeStart > timeout && timeout != -1);

  //messages and function stop
  if(armCannotMove)
  {
    logMessage("Step %d: Continued since the arm was not moving.\n", auton.step);
    auton.armCannotMove = true;
  }
  if(timeoutReached)
  {
    logMessage("Step %d: Arm movment timed out.\n", auton.step);
    auton.armTimedOut = true;
  }

  return positionReached || armCannotMove || timeoutReached;
}

bool intakeAuton(double power, bool hold/* = true*/, int timeout/* = -1*/)
{
  if(hold)
  {
    IntakeR.setBrake(vex::brakeType::brake);
    IntakeL.setBrake(vex::brakeType::brake);
  }
  else
  {
    IntakeR.setBrake(vex::brakeType::coast);
    IntakeL.setBrake(vex::brakeType::coast);
  }

  if(vex::timer::system() - auton.timeStart > timeout && timeout != -1)
  {
    intake.power = 0;
    return true;
  }

  intake.power = power;

  if(timeout == -1) return true;
  return false;
}

bool waitAuton(int time)
{
  return vex::timer::system() - auton.timeStart > time;
}

bool macroAuton(int macroId, int timeout/* = -1*/)
{
  if(auton.macroTimedOut)
  {
    macro.id = 0;
    doMacro();
    return true;
  }

  if(!auton.macroStarted)
  {
    macro.start = true;
    macro.id = macroId;
    macro.step = 0;
    auton.macroStarted = true;
  }

  doMacro();

  //exit conditions
  bool macroDone = macro.id == 0,
    timeoutReached = (vex::timer::system() - auton.timeStart > timeout && timeout != -1);

  //messages and function stop
  if(timeoutReached)
  {
    logMessage("Step %d: Macro id:%d timed out.\n", auton.step, macroId);
    auton.macroTimedOut = true;
  }

  return macroDone || timeoutReached;
}