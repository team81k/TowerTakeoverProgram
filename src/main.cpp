#include "vex.h"
#define EXTERN81k
#include "variables.h"

void updateControllerScreen()
{
  ControllerMaster.Screen.clearScreen();

  //main mode
  ControllerMaster.Screen.setCursor(1, 0);
  std::string modeString = "__Unknown Mode__";
  if(mainMode == 0) modeString = "__Stack Mode__";
  if(mainMode == 1) modeString = "__Tower Mode__";
  if(mainMode == 2) modeString = "__Manual Mode__";
  ControllerMaster.Screen.print("%s", modeString.c_str());

  //drive mode
  ControllerMaster.Screen.setCursor(2, 0);
  std::string driveTypeString = "None";
  if(drive.type == 0) driveTypeString = "Tank";
  if(drive.type == 1) driveTypeString = "Arcade";
  std::string driveSpeedString = "Fast";
  if(drive.slow) driveSpeedString = "Slow";
  ControllerMaster.Screen.print("D: %s, %s", driveTypeString.c_str(), driveSpeedString.c_str());

  ControllerPartner.Screen.clearScreen();

  //drive speed
  ControllerPartner.Screen.setCursor(2, 0);
  ControllerPartner.Screen.print("D: %s", driveSpeedString.c_str());

  //master modes
  ControllerPartner.Screen.setCursor(3, 0);
  std::string mode2String = "Unknown";
  if(mainMode == 0) mode2String = "Stack";
  if(mainMode == 1) mode2String = "Tower";
  if(mainMode == 2) mode2String = "Manual";
  ControllerPartner.Screen.print("M: (%s, %s)", mode2String.c_str(), driveTypeString.c_str());
}

void autonomous(void)
{
  auton.step = 0;
  autonomousRun();
}

void usercontrol(void)
{
  updateControllerScreen();

  while(true)
  {
    //controls
    ctrM.doControlCycle();
    ctrP.doControlCycle();
    ctrS.doControlCycle();

    handleSensorValues();

    //main modes/drive modes
    if(ControllerMaster.ButtonY.pressing() && ctrM.x.s == 1) {drive.type = 0;updateControllerScreen();}
    else if(ctrM.x.s == 1)//stack
    {
      mainMode = 0;
      updateControllerScreen();
    }

    if(ControllerMaster.ButtonY.pressing() && ctrM.a.s == 1) {drive.type = 1;updateControllerScreen();}
    else if(ctrM.a.s == 1)//tower
    {
      mainMode = 1;
      updateControllerScreen();
    }
    if(ctrM.b.s == 1)//manual
    {
      mainMode = 2;
      updateControllerScreen();
      macro.id = 0;
    }

    //macro
    if(ctrM.y.s == 1) macro.id = 0;

    if(mainMode == 0) userStackMode();
    if(mainMode == 1) userTowerMode();
    if(mainMode == 2) userManualMode();
    userPartnerControls();

    //drive
    if(!macro.useDrive)
    {
      if(drive.type == 0)//tank
      {
        drive.rightPower = ControllerMaster.Axis2.position();
        drive.leftPower = ControllerMaster.Axis3.position();
      }
      if(drive.type == 1)//arcade
      {
        drive.rightPower = ControllerMaster.Axis2.position() - ControllerMaster.Axis4.position() * 0.75;
        drive.leftPower = ControllerMaster.Axis2.position() + ControllerMaster.Axis4.position() * 0.75;
      }

      //partner arcade
      drive.rightPower += ControllerPartner.Axis2.position() - ControllerPartner.Axis4.position() * 0.75;
      drive.leftPower += ControllerPartner.Axis2.position() + ControllerPartner.Axis4.position() * 0.75;

      if(drive.slow)
      {
        drive.rightPower = drive.rightPower * 0.25;
        drive.leftPower = drive.leftPower * 0.25;
      }
    }

    doMacro();
    applyMotorPower();

    while(uiControls())
    {
      //controls
      ctrM.doControlCycle();
      ctrP.doControlCycle();
      ctrS.doControlCycle();

      //stop all motors
      drive.rightPower = 0;
      drive.leftPower = 0;
      tilter.power = 0;
      arm.power = 0;
      intake.power = 0;
      applyMotorPower();

      vex::task::sleep(7);
    }

    vex::task::sleep(7);
  }
}

int main()
{
  vex::thread(handleTimed).detach();

  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  addAuton("None", 0, "Does nothing.", 0);
  //addAuton("Red Large Goal", 1, "Scores in the large red goal zone.", 1);
  addAuton("Red Small Goal (6)", 1, "Scores 6 cubes in the small red goal zone.", 2);
  //addAuton("Blue Large Goal", 2, "Scores in the large blue goal zone.", 3);
  addAuton("Blue Small Goal (6)", 2, "Scores 6 cubes in the small blue goal zone.", 4);
  addAuton("Red Small Goal (7)", 1, "Scores 8 cubes in the small red goal zone. --Do_Not_Use", 5);
  addAuton("Blue Small Goal (7)", 2, "Scores 8 cubes in the small blue goal zone. --Do_Not_Use", 6);
  addAuton("Skills: Red Small Goal", 1, "Skills, starts on red goal zone.", 7);
  addAuton("Skills: Blue Small Goal", 2, "Skills, starts on blue goal zone.", 8);

  while(true)
  {
    if(!Competition.isEnabled() && (Competition.isCompetitionSwitch() || Competition.isFieldControl()))
    {
      //controls
      ctrM.doControlCycle();
      ctrP.doControlCycle();
      ctrS.doControlCycle();

      uiControls();
    }
    drawUi();
    vex::task::sleep(7);
  }
}
