#include "vex.h"
#include "variables.h"

void autonomousRun()
{
  //reset values
  auton.drivePartWay = false;
  nextAutonomousStep();
  auton.endMessage = "";
  auton.step = 0;
  useAutonInaccurateDrive = false;

  while(true)
  {
    handleSensorValues();

    if(auton.id == 1 || auton.id == 3)
    {
      int turn = 1;
      if(auton.id == 2) turn = -1;

      a_start;
      //a_(armAuton(800, 100, 10, true, 2500))_a;
      //a_(armAuton(armOpenPosition, 100, 10, true, 1000))_a;
      a_(driveAutonMove(950, 8, 50, 5000) & tilterAuton(1225, 50, 100, true, 0, 5000) & intakeAuton(-100) & (armAuton(armOpenPosition)_x))_a;
      a_(tilterAuton(40, 50, 100, false, 10, 3000), armAuton(armDefaultPosition, 100, 10, true, 3000) & intakeAuton(0))_a;
      a_(driveAutonMove(-750, 100, 50, 3000) & intakeAuton(25))_a;
      a_(driveAutonTurn(390 * turn, 100, 20, 4000))_a;
      a_(driveAutonMove(300, 50, 25, 1000))_a;
      a_(macroAuton(1))_a;
      a_end;
    }

    if(auton.id == 2 || auton.id == 4)
    {
      int turn = 1;
      if(auton.id == 2) turn = -1;

      //old 5 point
      /*a_start;
      //a_(driveAutonMovePart(1150, 80, 50, 125, 2000) & intakeAuton(100) & armAuton(armDefaultPosition, 10))_a;
      a_(driveAutonMove(1150, 35, 50, 4000) & intakeAuton(100) & armAuton(armDefaultPosition, 10))_a;
      a_(driveAutonMove(-750, 100, 50, 3000) & intakeAuton(25))_a;
      a_(driveAutonTurn(390 * turn, 100, 20, 4000))_a;
      a_(driveAutonMove(300, 50, 25, 1000))_a;
      a_(macroAuton(1))_a;
      a_end;*/

      a_start;
      a_(driveAutonMove(1200, 35, 50, 4000) & intakeAuton(100) & armAuton(armDefaultPosition))_a;
      a_(driveAutonTurn(-60 * turn, 100, 20, 4000))_a;
      a_(driveAutonMove(250, 35, 50, 1000))_a;
      a_(driveAutonMove(-775, 100, 50, 3000) & intakeAuton(25))_a;
      a_(driveAutonTurn(535 * turn, 100, 20, 4000))_a;
      a_(driveAutonMove(300, 35, 25, 1000))_a;
      a_(macroAuton(5))_a;
      a_end;
    }

    if(auton.id == 5 || auton.id == 6)
    {
      int turn = 1;
      if(auton.id == 5) turn = -1;

      useAutonInaccurateDrive = true;
      a_start;
      a_(driveAutonMove(760, 45, 50, 4000, 35) & armAuton(0, 10, 1000) & intakeAuton(100))_a;
      a_(driveAutonTurn(-160 * turn, 100, 15, 3000, 20) & (armAuton(0)_x))_a;
      a_(driveAutonMove(-900, 100, 50, 4500, 35))_a;
      a_(driveAutonTurn(160 * turn, 100, 15, 3000, 20))_a;
      a_(driveAutonMove(1000, 35, 50, 4000, 35))_a;
      a_(driveAutonMove(-770, 100, 50, 3000, 35) & intakeAuton(25))_a;
      a_(driveAutonTurn(395 * turn, 100, 15, 4000, 20))_a;
      a_(driveAutonMove(300, 100, 50, 1500))_a;
      a_(macroAuton(4))_a;
      a_end;
    }

    if(auton.id == 7 || auton.id == 8)
    {
      int turn = 1;
      if(auton.id == 7) turn = -1;

      a_start;
      a_(driveAutonMove(3225, 40, 25, 12000) & armAuton(0, 10, 1000) & intakeAuton(100))_a;
      //a_(driveAutonTurn(-20 * turn, 50, 10, 1000))_a;
      //a_(driveAutonMove(250, 50, 25, 1000))_a;
      //a_(driveAutonTurn(20 * turn, 50, 10, 1000))_a;
      //a_(driveAutonMove(1350, 50, 25, 6000) & armAuton(0, 10, 1000) & intakeAuton(100))_a;
      a_(driveAutonTurn(150 * turn, 50, 10, 2000))_a;
      a_(driveAutonMove(370, 50, 25, 2000))_a;
      a_(macroAuton(1))_a;
      a_(driveAutonTurn(-435 * turn, 100, 10, 2000) & intakeAuton(100))_a;

      a_(driveAutonMove(800, 100, 25, 4000))_a;
      a_(waitAuton(750))_a;
      a_(driveAutonMove(-100, 40, 25, 1000) & intakeAuton(-25, true, 800))_a;
      a_(armAuton(850))_a;
      a_(driveAutonMove(100, 40, 25, 1000) & (armAuton(900)_x))_a;
      a_(intakeAuton(-50, true, 1000) & (armAuton(900)_x))_a;
      a_(driveAutonMove(-600, 100, 25, 3000))_a;
      a_(driveAutonTurn(-300 * turn, 100, 10, 2000) & intakeAuton(100))_a;

      a_(driveAutonMove(650, 100, 25, 4000))_a;
      a_(waitAuton(750))_a;
      a_(driveAutonMove(-100, 40, 25, 1000) & intakeAuton(-25, true, 800))_a;
      a_(armAuton(850))_a;
      a_(driveAutonMove(100, 40, 25, 1000) & (armAuton(850)_x))_a;
      a_(intakeAuton(-50, true, 1000) & (armAuton(850)_x))_a;
      a_(driveAutonMove(-750, 100, 25, 3000))_a;
      a_end;
    }

    applyMotorPower();
  }

  macro.id = 0;
  auton.done = true;
}