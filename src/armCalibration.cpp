#include "vex.h"
#include "variables.h"

bool armCalibration()
{
  if(arm.calibrationStep == 0)
  {
    if(ArmPot.value(vex::rotationUnits::raw) > 4000) arm.power = 25;
    else if(ArmPot.value(vex::rotationUnits::raw) < 100) arm.power = -25;
    else
    {
      arm.power = 0;
      arm.calibrationStep++;
    }
  }
  else if(arm.calibrationStep == 1)
  {
    arm.offset -= arm.position
      - (162.0 - ArmPot.value(vex::rotationUnits::deg)) //pot offset
      * (84.0 / 12.0); //gearing
    arm.calibrationStep++;
  }
  else return false;

  return true;
}