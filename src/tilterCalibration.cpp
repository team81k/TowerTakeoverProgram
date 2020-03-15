#include "vex.h"
#include "variables.h"

bool tilterCalibration()
{
  if(tilter.calibrationStep == 0) //250 millis timeout to speed up
  {
    if(tilter.zeroCalStart == -1) tilter.zeroCalStart = vex::timer::system();
    if(vex::timer::system() - tilter.zeroCalStart > 250) tilter.calibrationStep++;
    tilter.power = -15;
  }
  else if(tilter.calibrationStep == 1) //wait for speed to be zero for 250 millis
  {
    tilter.power = -10;
    if(fabs(tilter.speed) < 10)
    {
      if(tilter.zeroSpeedStart == -1) tilter.zeroSpeedStart = vex::timer::system();
      if(vex::timer::system() - tilter.zeroSpeedStart > 250)
      {
        tilter.power = 0;
        tilter.zeroCalStart = vex::timer::system();
        tilter.calibrationStep++;
      }
    }
    else tilter.zeroSpeedStart = -1;
  }
  else if(tilter.calibrationStep == 2) //500 millis timeout to 'spring' back
  {
    tilter.power = std::max(500 - (int)(vex::timer::system() - tilter.zeroCalStart), 0) * -(5.0 / 500.0);
    if(vex::timer::system() - tilter.zeroCalStart > 500)
    {
      tilter.offset -= tilter.position;
      tilter.calibrationStep++;
    }
  }
  else return false;

  return true;
}