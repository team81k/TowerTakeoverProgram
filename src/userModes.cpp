#include "vex.h"
#include "variables.h"

void userStackMode()
{
  //stacking macro
  //if(ctrM.l1.s == 1) macro.id = 1;
  if(ctrM.l2.s == 1) macro.id = 2;
  if(ctrM.r1.s == 1) macro.id = 0;
  if(ctrM.r2.s == 1) macro.id = 0;

  //tilter control
  if(!macro.useTilter && !tilterCalibration())
  {
    tilter.slew = true;
    tilter.slewRate = 75;
    if(ControllerMaster.ButtonR1.pressing()) tilter.preSlewPower = (40 - tilter.position) * 0.06;
    else tilter.preSlewPower = 0;
  }

  //arm control
  if(!macro.useArm && !armCalibration())
  {
    if(ControllerMaster.ButtonR1.pressing()) arm.power = (armDefaultPosition - arm.position) * 0.3 - arm.speed * 0.02;
    else arm.power = 0;
  }

  //intake control
  if(!macro.useIntakes)
  {
    if(ControllerMaster.ButtonDown.pressing()) intakeMode = 0;
    if(ControllerMaster.ButtonUp.pressing()) intakeMode = 1;
    if(ControllerMaster.ButtonRight.pressing()) intakeMode = 2;

    if(ControllerMaster.ButtonLeft.pressing()) {intake.power = -50;intakeMode = 0;}
    else if(intakeMode == 1) intake.power = 100;
    else if(intakeMode == 2) intake.power = 25;
    else intake.power = 0;
  }
}

void userTowerMode()
{
  //tilter and arm control
  if(ctrM.l1.s == 1) {towerMode = 1;macro.id = 0;}
  if(ctrM.l2.s == 1) {towerMode = 2;macro.id = 0;}
  if(ctrM.r1.s == 1) {towerMode = 3;macro.id = 0;}
  if(ctrM.r2.s == 1) {towerMode = 0;macro.id = 3;}

  if(!macro.useTilter && !macro.useArm && !tilterCalibration() && !armCalibration())
  {
    //ignore exit conditions
    auton.tilterCannotMove = false;
    auton.tilterTimedOut = false;
    auton.armCannotMove = false;
    auton.armTimedOut = false;

    if(towerMode == 1) {armAuton(765);tilterAuton(0);}
    if(towerMode == 2) {armAuton(900);tilterAuton(0);}
    if(towerMode == 3) {armAuton(1500, 50);tilterAuton(1300);}
    if(towerMode == 0) {tilter.power = 0;arm.power = 0;}
  }

  //intake control
  if(!macro.useIntakes)
  {
    if(ControllerMaster.ButtonDown.pressing()) intakeMode = 0;
    if(ControllerMaster.ButtonUp.pressing()) intakeMode = 1;

    if(ControllerMaster.ButtonRight.pressing()) {intake.power = -15;intakeMode = 0;}
    else if(ControllerMaster.ButtonLeft.pressing()) {intake.power = -50;intakeMode = 0;}
    else if(intakeMode == 1) intake.power = 100;
    else if(intakeMode == 2) intake.power = 25;
    else intake.power = 0;
  }
}

void userManualMode()
{
  towerMode = 0;

  //tilter control
  if(!macro.useTilter && !tilterCalibration())
  {
    tilter.slew = true;
    tilter.slewRate = 75;
    if(ControllerMaster.ButtonL1.pressing())
      tilter.preSlewPower = ((1200.0 + 90.0) - tilter.position) * /*0.035*/0.048
      - std::abs(tilter.speed) * /*0.015*/0.0065;
    else if(ControllerMaster.ButtonL2.pressing()) tilter.preSlewPower = (0 - tilter.position) * 0.06;
    else tilter.preSlewPower = 0;
  }

  //arm control
  if(!macro.useArm && !armCalibration())
  {
    if(ControllerMaster.ButtonR1.pressing()) arm.power = 100;
    else if(ControllerMaster.ButtonR2.pressing()) arm.power = -50;
    else if(arm.position > 315) arm.power = fabs(std::sin(arm.absoluteAngle * PI / 180.0)) * 0.6;
    else arm.power = 0;
  }

  //intake control
  if(!macro.useIntakes)
  {
    if(ControllerMaster.ButtonDown.pressing()) intakeMode = 0;
    if(ControllerMaster.ButtonUp.pressing()) intakeMode = 1;
    if(ControllerMaster.ButtonRight.pressing()) intakeMode = 2;

    if(ControllerMaster.ButtonLeft.pressing()) {intake.power = -50;intakeMode = 0;}
    else if(intakeMode == 1) intake.power = 100;
    else if(intakeMode == 2) intake.power = 25;
    else intake.power = 0;
  }
}

void userPartnerControls()
{
  //slow drive
  if(ctrP.x.s == 1) {drive.slow = true;updateControllerScreen();}
  if(ctrP.a.s == 1) {drive.slow = false;updateControllerScreen();}

  //stack get macros
  if(ctrP.l1.s == 1) macro.id = 6;
  if(ctrP.l2.s == 1) macro.id = 2;
  if(ctrP.r1.s == 1) macro.id = 7;
  if(ctrP.r2.s == 1) macro.id = 0;

  //intake control
  if(!macro.useIntakes)
  {
    if(ControllerPartner.ButtonDown.pressing()) intakeMode = 0;
    if(ControllerPartner.ButtonUp.pressing()) intakeMode = 1;
    if(ControllerPartner.ButtonRight.pressing()) intakeMode = 2;

    if(ControllerPartner.ButtonLeft.pressing()) {intake.power = -100;intakeMode = 0;}
    if(ControllerPartner.ButtonB.pressing()) {intake.power = -15;intakeMode = 0;}
    else if(intakeMode == 1) intake.power = 100;
    else if(intakeMode == 2) intake.power = 25;
    //else intake.power = 0;//userModes reset to zero
  }

  //tilter recalibrate
  if(ControllerPartner.ButtonX.pressing() && ControllerPartner.ButtonA.pressing() &&
    ControllerPartner.ButtonB.pressing() && ControllerPartner.ButtonY.pressing())
  {
    tilter.calibrationStep = 0;
    tilter.zeroCalStart = -1;
    tilter.zeroSpeedStart = -1;
  }
}