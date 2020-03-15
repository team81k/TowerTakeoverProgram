#include "vex.h"
#include "variables.h"

void doMacro()
{
  //fullstack deploy
  if(macro.id == 1)
  {
    am_reserve(true, true, true, true);
    a_start;
    am_(tilterAutonPart(1350, 125, 1000) & intakeAuton(-6))_ma;
    am_(tilterAuton(1350, 125) & intakeAuton(0) & armAuton(armOpenPosition))_ma;
    am_(intakeAuton(-50) & driveAutonMove(-500, 10, 25, 1500))_ma;
    am_(tilterAuton(0) & armAuton(armDefaultPosition))_ma;
    am_end;
  }

  //low drop stack deposit
  if(macro.id == 2)
  {
    am_reserve(true, true, true, true);
    a_start;
    am_(tilterAutonPart(1350, 25, 1000) & intakeAuton(0))_ma;
    am_(tilterAuton(1350, 125) & intakeAuton(-15) & armAuton(armOpenPosition))_ma;
    am_(intakeAuton(-50) & driveAutonMove(-500, 10, 25, 1500))_ma;
    am_(tilterAuton(0) & armAuton(armDefaultPosition))_ma;
    am_end;
  }

  //bring down
  if(macro.id == 3)
  {
    am_reserve(false, true, true, false);
    a_start;
    am_(armAuton(armDefaultPosition, 40) & tilterAuton(40))_ma;
    am_end;
  }

  //small stack deploy
  if(macro.id == 4)
  {
    am_reserve(true, true, true, true);
    a_start;
    am_(tilterAutonPart(1480, 650, 1000, 100, false, 7) & intakeAuton(-20))_ma;
    am_(tilterAuton(1480, 650, 100, false, 7) & intakeAuton(0) & armAuton(armOpenPosition))_ma;
    am_(intakeAuton(-50) & driveAutonMove(-500, 10, 25, 1500))_ma;
    am_(tilterAuton(0) & armAuton(armDefaultPosition))_ma;
    am_end;
  }

  //small stack deploy (pick up position)
  if(macro.id == 5)
  {
    am_reserve(true, true, true, true);
    a_start;
    am_(tilterAutonPart(1480, 650, 1000, 100, false, 7) & intakeAuton(-20))_ma;
    am_(tilterAuton(1480, 650, 100, false, 7) & intakeAuton(0) & armAuton(armOpenPosition))_ma;
    am_(intakeAuton(-50) & driveAutonMove(-800, 10, 25, 2000))_ma;
    am_end;
  }

  //cube pick up
  if(macro.id == 6)
  {
    am_reserve(false, true, true, false);
    a_start;
    am_(tilterAuton(1230, 50, 100, true, 10, 2500) & armAuton(armOpenPosition, 100, 10, true, 2500))_ma;
    am_end;
  }

  //bring stack down
  if(macro.id == 7)
  {
    am_reserve(false, true, true, false);
    a_start;
    am_(armAuton(armDefaultPosition))_ma;
    am_(tilterAutonPart(40, 50, 300, 15) & (armAuton(armDefaultPosition)_x))_ma;
    am_(tilterAuton(40, 50, 25) & (armAuton(armDefaultPosition)_x))_ma;
    am_end;
  }

  if(macro.id == 0)
  {
    macro.start = true;
    macro.step = 0;
    macro.useDrive = false;
    macro.useTilter = false;
    macro.useArm = false;
    macro.useIntakes = false;
  }
}