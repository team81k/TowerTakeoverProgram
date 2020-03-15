#include "vex.h"

#pragma once

#ifndef EXTERN81k
#define EXTERN81k extern
#define EXTERNarg(a) 
#else
#define EXTERNarg(a) a
#endif

#define PI 3.14159265358979323846

EXTERN81k void handleSensorValues();
EXTERN81k void applyMotorPower();
EXTERN81k void handleTimed();
EXTERN81k void doMacro();
EXTERN81k bool tilterCalibration();
EXTERN81k bool armCalibration();
EXTERN81k void addAuton(std::string name, int color, std::string description, int id);
EXTERN81k bool uiControls();
EXTERN81k void drawUi();
EXTERN81k void updateControllerScreen();
EXTERN81k void autonomousRun();

EXTERN81k void userStackMode();
EXTERN81k void userTowerMode();
EXTERN81k void userManualMode();
EXTERN81k void userPartnerControls();

EXTERN81k void logMessage(const char* format, ...);
EXTERN81k void nextAutonomousStep(bool newTime = true, bool isMacro = false);
//EXTERN81k bool driveAutonSimple(int dist, bool turn, double kP, double kD, double aKP, double slewRate, int maxSpeed, int threashold, int timeout, double add);
EXTERN81k bool driveAutonMove(double dist, double maxSpeed = 100, double threashold = 25, int timeout = -1, double add = 0);
EXTERN81k bool driveAutonTurn(double dist, double maxSpeed = 50, double threashold = 10, int timeout = -1, double add = 0);
EXTERN81k bool driveAutonMovePart(double dist, double maxSpeed, double threashold, double position, int timeout = -1, double add = 0);
EXTERN81k bool driveAutonTurnPart(double dist, double maxSpeed, double threashold, double position, int timeout = -1, double add = 0);
EXTERN81k bool tilterAuton(double position, double threashold = 50, double maxSpeed = 100, bool fastForward = false, int cubeCount = 10, int timeout = -1);
EXTERN81k bool tilterAutonPart(double position, double threashold, double endPosition, double maxSpeed = 100, bool fastForward = false, int cubeCount = 10, int timeout = -1);
EXTERN81k bool armAuton(double position, double maxSpeed = 100, double threashold = 10, bool useHoldPower = true, int timeout = -1);
EXTERN81k bool intakeAuton(double power, bool hold = true, int timeout = -1);
EXTERN81k bool waitAuton(int time);
EXTERN81k bool macroAuton(int macroId, int timeout = -1);

EXTERN81k bool useAutonInaccurateDrive EXTERNarg(= false);
#define am_reserve(a_drive, a_tilter, a_arm, a_intakes) if(macro.start){nextAutonomousStep(true, true);macro.start = false;macro.step = 0;macro.useDrive = a_drive;macro.useTilter = a_tilter;macro.useArm = a_arm;macro.useIntakes = a_intakes;}
#define a_start int aSI = 0;
#define a_ if(auton.step == aSI++) { if
#define am_ if(macro.step == aSI++) { if
#define _a nextAutonomousStep(); }
#define _b nextAutonomousStep(false); }
#define _ma nextAutonomousStep(true, true); }
#define a_end if(auton.step == aSI++) break;
#define am_end if(macro.step == aSI++) macro.id = 0;

#define _x |1

EXTERN81k vex::competition Competition;
EXTERN81k vex::brain Brain;
EXTERN81k vex::controller ControllerMaster EXTERNarg((vex::primary));
EXTERN81k vex::controller ControllerPartner EXTERNarg((vex::partner));

EXTERN81k vex::motor FR EXTERNarg((14, vex::ratio18_1, false));
EXTERN81k vex::motor FL EXTERNarg((1, vex::ratio18_1, true));
EXTERN81k vex::motor BR EXTERNarg((2, vex::ratio18_1, false));
EXTERN81k vex::motor BL EXTERNarg((3, vex::ratio18_1, true));

//folded = 255
//intake = 0
//let go = -450
EXTERN81k double armOpenPosition EXTERNarg( = -480);
EXTERN81k double armDefaultPosition EXTERNarg( = 115);
EXTERN81k double armFoldedPosition EXTERNarg( = 255);
EXTERN81k vex::motor ArmR EXTERNarg((15, vex::ratio18_1, true));
EXTERN81k vex::motor ArmL EXTERNarg((13, vex::ratio18_1, false));

EXTERN81k vex::motor IntakeL  EXTERNarg((11, vex::ratio18_1, false));
EXTERN81k vex::motor IntakeR  EXTERNarg((12, vex::ratio18_1, true));

EXTERN81k vex::pot ArmPot  EXTERNarg((Brain.ThreeWirePort.A));

EXTERN81k int screenWidth EXTERNarg( = 480);
EXTERN81k int screenHeight  EXTERNarg( = 240);

EXTERN81k lib81k::controlsController ctrM EXTERNarg((ControllerMaster));
EXTERN81k lib81k::controlsController ctrP EXTERNarg((ControllerPartner));
EXTERN81k lib81k::controlsScreen ctrS EXTERNarg((Brain.Screen));

//0 stack
//1 tower
//2 manual
EXTERN81k int mainMode EXTERNarg( = 0);
EXTERN81k int intakeMode EXTERNarg( = 0);
EXTERN81k int towerMode EXTERNarg( = 0);

EXTERN81k struct driveStruct {
  int type = 1;
  double rightPower = 0, preSlewRightPower = 0;
  double leftPower = 0, preSlewLeftPower = 0;
  bool slow = false, slew = false;
  double slewRate = 100;
  double rightPosition = 0, rightPrePosition = 0;
  double leftPosition = 0, leftPrePosition = 0;
  double rightSpeed = 0;
  double leftSpeed = 0;
} drive;

EXTERN81k struct tilterStruct {
  double position = 0, prePosition = 0, offset = 0;
  double speed = 0;
  double power = 0, preSlewPower;
  double angle = 0;
  bool slew = false;
  double slewRate = 100;
  int calibrationStep = 0;
  int zeroCalStart = -1;
  int zeroSpeedStart = -1;
} tilter;

EXTERN81k struct armStruct {
  double position = 0, offset = 0, prePosition = 0;
  int target = 0, error = 0;
  double speed = 0;
  double power = 0;
  double angle = 0;
  double absoluteAngle = 0;
  int calibrationStep = 0;
} arm;

EXTERN81k struct intakeStruct {
  double avgPosition = 0, avgPrePosition = 0;
  double avgSpeed = 0;
  double power = 0;
} intake;

EXTERN81k struct macroStruct {
  int id = 0;
  int step = 0;
  bool start = true;
  bool useDrive = false;
  bool useTilter = false;
  bool useArm = false;
  bool useIntakes = false;
  long timeStart = 0;
  int driveRightStart = 0;
  int driveLeftStart = 0;
} macro;

EXTERN81k struct autonStruct {
  int id = 0;
  int step = 0;
  bool done = false;
  std::string endMessage;

  //timeout
  long timeStart = 0;

  //drive
  long driveZeroErrorStart = -1;
  long driveZeroSpeedStart = -1;
  int driveRightStart = 0;
  int driveLeftStart = 0;
  bool driveCannotMove = false, driveTimedOut = false;
  bool drivePartWay = false;

  //tilter
  long tilterZeroSpeedStart = -1;
  bool tilterCannotMove = false, tilterTimedOut = false;

  //arm
  long armZeroErrorStart = -1;
  long armZeroSpeedStart = -1;
  bool armCannotMove = false, armTimedOut = false;

  //macro
  bool macroStarted = false;
  bool macroTimedOut = false;
} auton;

EXTERN81k unsigned long preTime EXTERNarg( = vex::timer::system());