#include "lib81k/controls.h"

/*
s = state
  0 = unpressed
  1 = first press cycle
  2 = pressed
  3 = first unpress cycle
*/

#define btnControl(btnS, btnST, btnC, btnCT, btnA, btnB, btnV) if(btnS != (btnV * 2)) btnS = (btnS + 1) % 4;\
if(btnS == 1 || btnS == 3) btnST = vex::timer::system();\
btnC = std::fmax(0, ceil(((long)vex::timer::system() - btnST - btnA) / (double)btnB) - ceil((btnCT - btnA) / (double)btnB));\
if(btnC > 0) btnCT = vex::timer::system() - btnST;\
if(btnS == 0) btnC = -btnC;\
if(btnS == 1 || btnS == 3) btnCT = btnA, btnC = (btnS == 1) ? 1 : -1;\

void lib81k::controlsController::doControlCycle()
{
  //controller button controls
  btnControl(x.s, x.sTime, x.c, x.cTime, 500, 250, targetController->ButtonX.pressing())
  btnControl(a.s, a.sTime, a.c, a.cTime, 500, 250, targetController->ButtonA.pressing())
  btnControl(b.s, b.sTime, b.c, b.cTime, 500, 250, targetController->ButtonB.pressing())
  btnControl(y.s, y.sTime, y.c, y.cTime, 500, 250, targetController->ButtonY.pressing())
  btnControl(u.s, u.sTime, u.c, u.cTime, 500, 250, targetController->ButtonUp.pressing())
  btnControl(r.s, r.sTime, r.c, r.cTime, 500, 250, targetController->ButtonRight.pressing())
  btnControl(d.s, d.sTime, d.c, d.cTime, 500, 250, targetController->ButtonDown.pressing())
  btnControl(l.s, l.sTime, l.c, l.cTime, 500, 250, targetController->ButtonLeft.pressing())
  btnControl(l1.s, l1.sTime, l1.c, l1.cTime, 500, 250, targetController->ButtonL1.pressing())
  btnControl(l2.s, l2.sTime, l2.c, l2.cTime, 500, 250, targetController->ButtonL2.pressing())
  btnControl(r1.s, r1.sTime, r1.c, r1.cTime, 500, 250, targetController->ButtonR1.pressing())
  btnControl(r2.s, r2.sTime, r2.c, r2.cTime, 500, 250, targetController->ButtonR2.pressing())
}

void lib81k::controlsScreen::doControlCycle()
{
  //touch screen controls
  if(state != (targetScreen->pressing() * 2)) state = (state + 1) % 4;
  if(state == 1)
  {
    startX = preX = targetScreen->xPosition();
    startY = preY = targetScreen->yPosition();
    totalMovement = 0;
  }
  if(state == 2)
  {
    moveX = preX - targetScreen->xPosition();
    moveY = preY - targetScreen->xPosition();
    preX = targetScreen->xPosition();
    preY = targetScreen->yPosition();
    totalMovement = std::fmax(totalMovement, sqrt(pow(startX - targetScreen->xPosition(), 2) + pow(startY - targetScreen->yPosition(), 2)));
  }
}