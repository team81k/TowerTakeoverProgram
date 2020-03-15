#include "vex.h"
#include "variables.h"

#define clickInside(x1, y1, width, height)\
(Brain.Screen.xPosition() >= x1 && Brain.Screen.xPosition() <= x1 + width\
&& Brain.Screen.yPosition() >= y1 && Brain.Screen.yPosition() <= y1 + height)

long lastVisualUpdate = vex::timer::system();
double easeS0 = 0, easeS1 = 1.5, easeS2 = 1.5, easeS3 = 1.5, easeS4 = 1.5;
int uiState = 0;
double selectedOffset = 0, selectedOffsetTarget = 0;
int selectedAuton = 0, selectedAutonMode = 0;
long autonStart, autonEnd;
bool wasAuton = false;

struct AutonData
{
  std::string name = "";
  int color = 0;
  std::string description = "";
  int id = 0;
};

std::vector<AutonData> autonOptions;

void addAuton(std::string name, int color, std::string description, int id)
{
  AutonData newAuton;
  newAuton.name = name;
  newAuton.color = color;
  newAuton.description = description;
  newAuton.id = id;
  autonOptions.push_back(newAuton);
}

bool uiControls()
{
  if(uiState == 0)
  {
    if(ControllerMaster.ButtonUp.pressing() && ControllerMaster.ButtonRight.pressing()
      && ControllerMaster.ButtonDown.pressing() && ControllerMaster.ButtonLeft.pressing()) uiState = 1;

    if(ctrS.state == 3 && ctrS.totalMovement < 15)
    {
      if(clickInside(screenWidth - 160, 0 - easeS0 * 50, 160, 50)) uiState = 1;
    }
  }
  else if(uiState == 1)
  {
    if(ctrM.x.s == 1 && Competition.isEnabled()) uiState = 0;
    if(ctrM.a.s == 1 && Competition.isEnabled()) uiState = 2;

    if(ctrS.state == 3 && ctrS.totalMovement < 15)
    {
      if(clickInside(0, 0 - easeS1 * 50, 115, 50) && Competition.isEnabled()) uiState = 0;
      if(clickInside(screenWidth - 140, 0 - easeS1 * 50, 140, 50) && Competition.isEnabled()) uiState = 2;
    }

    //arrow controls
    if(ctrM.r.c > 0) selectedAuton = std::fmin(selectedAuton + ctrM.r.c, autonOptions.size() - 1);
    if(ctrM.l.c > 0) selectedAuton = std::fmax(selectedAuton - ctrM.l.c, 0);
    if(ctrM.r.c > 0 || ctrM.l.c > 0)
    {
      if(selectedAuton * 160 < selectedOffsetTarget) selectedOffsetTarget = selectedAuton * 160;
      if(selectedAuton * 160 - 2 * 160 > selectedOffsetTarget) selectedOffsetTarget = selectedAuton * 160 - 2 * 160;
      selectedOffsetTarget -= fmod(selectedOffsetTarget, 160);
    }

    //touch controls
    if(ctrS.state == 2 && ctrS.startY > 60 && ctrS.startY < 180) selectedOffset = selectedOffsetTarget += ctrS.moveX;
    if(ctrS.state == 3 && ctrS.startY > 60 && ctrS.startY < 180)
    {
      if(selectedOffset < 0) selectedOffsetTarget = 0;
      if(selectedOffsetTarget > (autonOptions.size() - 3) * 160) selectedOffsetTarget = (autonOptions.size() - 3) * 160;
      if(ctrS.totalMovement < 15 && ctrS.startY > 60 && ctrS.startY < 180) selectedAuton = (Brain.Screen.xPosition() + selectedOffset) / 160;
    }

    //set auton
    auton.id = autonOptions[selectedAuton].id;
  }
  else if(uiState == 2)
  {
    if(ctrM.x.s == 1) uiState = 1;
    if(ctrM.a.s == 1) uiState = 3;

    if(ctrM.l.c > 0) selectedAutonMode--;
    if(ctrM.r.c > 0) selectedAutonMode++;
    if(selectedAutonMode < 0) selectedAutonMode = 0;
    if(selectedAutonMode > 2) selectedAutonMode = 2;

    if(ctrS.state == 3 && ctrS.totalMovement < 15)
    {
      if(clickInside(0, 0 - easeS2 * 50, 100, 50)) uiState = 1;
      if(clickInside(12 + 0 * 156 - easeS2 * screenWidth, 125, 144, 75)) selectedAutonMode = 0;
      if(clickInside(12 + 1 * 156 - easeS2 * screenWidth, 125, 144, 75)) selectedAutonMode = 1;
      if(clickInside(12 + 2 * 156 - easeS2 * screenWidth, 125, 144, 75)) selectedAutonMode = 2;
    }
  }
  else if(uiState == 3)
  {
    autonStart = vex::timer::system();
    int timeLimit = 15;
    if(selectedAutonMode == 1) timeLimit = 60;
    if(selectedAutonMode == 2) timeLimit = -1;
    auton.id = autonOptions[selectedAuton].id;
    auton.step = 0;
    auton.done = false;
    vex::thread autonThread(autonomousRun);

    while(true)
    {
      //controls
      ctrM.doControlCycle();
      ctrP.doControlCycle();
      ctrS.doControlCycle();

      if(vex::timer::system() - autonStart > timeLimit * 1000 && timeLimit != -1) break;
      if(ctrM.x.s == 1) break;
      if(auton.done) break;
    }

    autonThread.interrupt();
    autonEnd = vex::timer::system();

    uiState = 4;
    macro.id = 0;
    drive.slew = false;
  }
  else if(uiState == 4)
  {
    if(ctrM.x.s == 1) uiState = 0;
    if(ctrM.b.s == 1) uiState = 1;
    if(ctrM.a.s == 1) uiState = 3;

    if(ctrS.state == 3 && ctrS.totalMovement < 15)
    {
      if(clickInside(0, 0 - easeS4 * 50, 115, 50)) uiState = 0;
      if(clickInside(0, screenHeight - 50 + easeS4 * 50, 100, 50)) uiState = 1;
    }
  }

  if(uiState == 0) return false;
  return true;
}

void drawUi()
{
  bool isComp = Competition.isCompetitionSwitch() || Competition.isFieldControl();
  if(isComp)
  {
    if(!Competition.isEnabled()) {wasAuton = true;uiState = 1;}
    if(Competition.isEnabled() && uiState == 1 && wasAuton) {wasAuton = false;uiState = 0;}
  }

  Brain.Screen.clearScreen(vex::white);

  double dT = std::fmin(1.0, (vex::timer::system() - lastVisualUpdate) / 150.0);
  lastVisualUpdate = vex::timer::system();
  easeS0 += ((uiState == 0 ? 0 : 1.5) - easeS0) * dT;
  easeS1 += ((uiState == 1 ? 0 : 1.5) - easeS1) * dT;
  easeS2 += ((uiState == 2 ? 0 : 1.5) - easeS2) * dT;
  easeS3 += ((uiState == 3 ? 0 : 1.5) - easeS3) * dT;
  easeS4 += ((uiState == 4 ? 0 : 1.5) - easeS4) * dT;
  selectedOffset += (selectedOffsetTarget - selectedOffset) * dT;

  //usercontrol start ui
  Brain.Screen.setPenColor(vex::black);
  Brain.Screen.setFont(vex::mono20);
  Brain.Screen.printAt(10 - easeS0 * screenWidth, 85, false, "Tilter = [ cs: %d, pos: %4.2f,", tilter.calibrationStep, tilter.position);
  Brain.Screen.printAt(10 - easeS0 * screenWidth, 115, false, "    speed: %4.1f, pow: %4.2f, angle: %f ]", tilter.speed, tilter.power, tilter.angle);
  Brain.Screen.printAt(10 - easeS0 * screenWidth, 150, false, "Arm = [ pos: %4.2f, speed: %5.1f, pow: %3.2f, ", arm.position, arm.speed, arm.power);
  Brain.Screen.printAt(10 - easeS0 * screenWidth, 180, false, "    angle: %4.2f, pot: %4.2f, es: %4.2f ]", arm.absoluteAngle, ArmPot.value(vex::deg), ArmR.position(vex::rotationUnits::deg) - ArmL.position(vex::rotationUnits::deg));
  Brain.Screen.printAt(10 - easeS0 * screenWidth, 215, false, "Drive = [ FR: %3.0ff, FL: %3.0ff, BR: %3.0ff, BL: %3.0ff ]", FR.temperature(vex::temperatureUnits::fahrenheit), FL.temperature(vex::temperatureUnits::fahrenheit), BR.temperature(vex::temperatureUnits::fahrenheit), BL.temperature(vex::temperatureUnits::fahrenheit));

  //enter auton selector button
  Brain.Screen.setPenWidth(0);
  Brain.Screen.setFillColor(vex::color(50, 50, 50));
  Brain.Screen.drawRectangle(screenWidth - 200, 0 - easeS0 * 50, 200, 50);

  Brain.Screen.setPenColor(vex::white);
  Brain.Screen.setFont(vex::mono20);
  Brain.Screen.printAt(screenWidth - 190, 30 - easeS0 * 50, false, "Autonomous Selector");

  //main selection
  if(Competition.isEnabled())
  {
    Brain.Screen.setPenWidth(0);
    Brain.Screen.setFillColor(vex::color(150, 50, 50));
    Brain.Screen.drawRectangle(0, 0 - easeS1 * 50, 115, 50);
    Brain.Screen.setFillColor(vex::color(50, 150, 50));
    Brain.Screen.drawRectangle(screenWidth - 140, 0 - easeS1 * 50, 140, 50);

    Brain.Screen.setPenColor(vex::white);
    Brain.Screen.setFont(vex::mono20);
    Brain.Screen.printAt(10, 30 - easeS1 * 50, false, "Close (X)");
    Brain.Screen.printAt(screenWidth - 130, 30 - easeS1 * 50, false, "Continue (A)");
    Brain.Screen.setPenColor(vex::black);
    Brain.Screen.printAt(133, 30 - easeS1 * 50, false, "Autonomous Selector");
  }
  else
  {
    Brain.Screen.setPenColor(vex::black);
    Brain.Screen.setFont(vex::mono20);
    Brain.Screen.printAt(155, 30 - easeS1 * 50, false, "Autonomous Selector");
  }

  for(int i = 0; i < autonOptions.size(); i++)
  {
    int x = i * 160 + 10 - selectedOffset + easeS1 * (selectedOffset + screenWidth);

    Brain.Screen.setPenColor(vex::color(50, 50, 50));
    if(selectedAuton == i) Brain.Screen.setPenWidth(5);
    else Brain.Screen.setPenWidth(0);
    if(autonOptions[i].color == 0) Brain.Screen.setFillColor(vex::color(150, 150, 150));
    if(autonOptions[i].color == 1) Brain.Screen.setFillColor(vex::color(250, 150, 150));
    if(autonOptions[i].color == 2) Brain.Screen.setFillColor(vex::color(150, 150, 250));
    Brain.Screen.drawRectangle(x, 60, 140, screenHeight - 60 - 60);

    int textY = 80;
    std::string tempString = autonOptions[i].name;
    Brain.Screen.setPenColor(vex::black);
    Brain.Screen.setFont(vex::mono20);

    while(tempString.size() > 0 && textY < 170)
    {
      int printLength = std::min(12, (int)tempString.size());
      if(printLength == 12)
      {
        while(true)
        {
          if(tempString[printLength - 1] == ' ') break;
          else printLength--;
          if(printLength == 0) {printLength = 12;break;}
        }
      }
      Brain.Screen.printAt(x + 10, textY, false, tempString.substr(0, printLength).c_str());
      tempString.erase(0, printLength);
      textY += 20;
    }

    textY += 6;
    tempString = autonOptions[i].description;
    Brain.Screen.setPenColor(vex::color(50, 50, 50));
    Brain.Screen.setFont(vex::mono12);

    while(tempString.size() > 0 && textY < 170)
    {
      int printLength = std::min(21, (int)tempString.size());
      if(printLength == 21)
      {
        while(true)
        {
          if(tempString[printLength - 1] == ' ') break;
          else printLength--;
          if(printLength == 0) {printLength = 21;break;}
        }
      }
      Brain.Screen.printAt(x + 10, textY, false, tempString.substr(0, printLength).c_str());
      tempString.erase(0, printLength);
      textY += 12;
    }
  }

  Brain.Screen.setFillColor(vex::color(50, 50, 200));
  Brain.Screen.setPenWidth(0);
  int scrollStart = (selectedOffset / (autonOptions.size() * 160.0 + 20.0)) * (screenWidth - 40) + 15 + easeS1 * screenWidth;
  int scrollLength = (3.0 / autonOptions.size())  * (screenWidth - 40) + 15;
  Brain.Screen.drawRectangle(scrollStart, 200, scrollLength, 11);
  Brain.Screen.drawCircle(scrollStart, 205, 5);
  Brain.Screen.drawCircle(scrollStart + scrollLength, 205, 5);

  //auton type
  Brain.Screen.setPenWidth(0);
  Brain.Screen.setFillColor(vex::color(150, 50, 50));
  Brain.Screen.drawRectangle(0, 0 - easeS2 * 50, 100, 50);
  Brain.Screen.setFillColor(vex::color(240, 240, 240));
  Brain.Screen.drawRectangle(screenWidth - 110, 0 - easeS2 * 50, 110, 50);

  Brain.Screen.setPenColor(vex::white);
  Brain.Screen.setFont(vex::mono20);
  Brain.Screen.printAt(10, 30 - easeS2 * 50, false, "Back (X)");
  Brain.Screen.setPenColor(vex::black);
  Brain.Screen.printAt(screenWidth - 100, 30 - easeS2 * 50, false, "Start (A)");
  Brain.Screen.printAt(160, 80 - easeS2 * 100, false, "Autonomous Type");

  for(int i = 0; i < 3; i++)
  {
    Brain.Screen.setPenColor(vex::color(50, 50, 50));
    if(selectedAutonMode == i) Brain.Screen.setPenWidth(5);
    else Brain.Screen.setPenWidth(0);
    Brain.Screen.setFillColor(vex::color(200, 200, 200));
    Brain.Screen.drawRectangle(12 + i * 156 - easeS2 * screenWidth, 125, 144, 75);
    Brain.Screen.setPenColor(vex::black);
    Brain.Screen.setFont(vex::mono20);
    std::string text = "Normal";
    if(i == 1) text = "Skills";
    if(i == 2) text = "Unlimited";
    Brain.Screen.printAt(12 + i * 156 - easeS2 * screenWidth + 10, 155, false, "%s", text.c_str());
    std::string text2 = "15 seconds";
    if(i == 1) text2 = "60 seconds";
    if(i == 2) text2 = "∞ seconds";
    Brain.Screen.printAt(12 + i * 156 - easeS2 * screenWidth + 10, 185, false, "%s", text2.c_str());
  }

  //auton run
  if(Competition.isEnabled())
  {
    Brain.Screen.setPenWidth(0);
    Brain.Screen.setFillColor(vex::color(255, 200, 200));
    Brain.Screen.drawRectangle(0, 0 - easeS3 * 50, 100, 50);

    Brain.Screen.setPenColor(vex::black);
    Brain.Screen.setFont(vex::mono20);
    Brain.Screen.printAt(10, 30 - easeS3 * 50, false, "Stop (X)");

    Brain.Screen.setFont(vex::mono60);
    long elapsedTime = vex::timer::system() - autonStart;
    if(elapsedTime < 60 * 1000) Brain.Screen.printAt(30 + easeS3 * screenWidth, 150, false, "%.1f seconds", elapsedTime / 1000.0);
    else if(elapsedTime < 60 * 60 * 1000) Brain.Screen.printAt(30 + easeS3 * screenWidth, 150, false, "%.0f min %.0f sec", std::floor(elapsedTime / (60 * 1000.0)), std::fmod(elapsedTime / 1000.0, 60));
    else if(elapsedTime < 24 * 60 * 60 * 1000) Brain.Screen.printAt(30 + easeS3 * screenWidth, 150, false, "%.0f hours %.0f min", std::floor(elapsedTime / (60 * 60 * 1000.0)), std::fmod(elapsedTime / (60 * 1000.0), 60));
    else if(elapsedTime < 7 * 24 * 60 * 60 * 1000) Brain.Screen.printAt(30 + easeS3 * screenWidth, 150, false, "%.0f days %.0f hours", std::floor(elapsedTime / (24 * 60 * 60 * 1000.0)), std::fmod(elapsedTime / (60 * 60 * 1000.0), 24));
    else Brain.Screen.printAt(30 + easeS3 * screenWidth, 150, false, "%.0f years %.0f days", std::floor(elapsedTime / (356 * 24 * 60 * 60 * 1000.0)), std::fmod(elapsedTime / (24 * 60 * 60 * 1000.0), 365));
  }

  //auton after
  Brain.Screen.setPenWidth(0);
  Brain.Screen.setFillColor(vex::color(150, 50, 50));
  Brain.Screen.drawRectangle(0, 0 - easeS4 * 50, 115, 50);
  Brain.Screen.setFillColor(vex::color(50, 50, 50));
  Brain.Screen.drawRectangle(0, screenHeight - 50 + easeS4 * 50, 100, 50);
  Brain.Screen.setFillColor(vex::color(200, 255, 200));
  Brain.Screen.drawRectangle(screenWidth - 150, 0 - easeS4 * 50, 150, 50);

  Brain.Screen.setPenColor(vex::white);
  Brain.Screen.setFont(vex::mono20);
  Brain.Screen.printAt(10, 30 - easeS4 * 50, false, "Close (X)");
  Brain.Screen.printAt(10, screenHeight - 20 + easeS4 * 50, false, "Back (B)");
  Brain.Screen.setPenColor(vex::black);
  Brain.Screen.printAt(screenWidth - 140, 30 - easeS4 * 50, false, "Run Again (A)");

  Brain.Screen.printAt(30 - easeS4 * screenWidth, 100, false, "The autonomous lasted...");

  Brain.Screen.setFont(vex::mono30);
  long elapsedTime = autonEnd - autonStart;
  if(elapsedTime < 60 * 1000) Brain.Screen.printAt(30 - easeS4 * screenWidth, 150, false, "%.1f seconds", elapsedTime / 1000.0);
  else if(elapsedTime < 60 * 60 * 1000) Brain.Screen.printAt(30 - easeS4 * screenWidth, 150, false, "%.0f min %.0f sec", std::floor(elapsedTime / (60 * 1000.0)), std::fmod(elapsedTime / 1000.0, 60));
  else if(elapsedTime < 24 * 60 * 60 * 1000) Brain.Screen.printAt(30 - easeS4 * screenWidth, 150, false, "%.0f hours %.0f min", std::floor(elapsedTime / (60 * 60 * 1000.0)), std::fmod(elapsedTime / (60 * 1000.0), 60));
  else if(elapsedTime < 7 * 24 * 60 * 60 * 1000) Brain.Screen.printAt(30 - easeS4 * screenWidth, 150, false, "%.0f days %.0f hours", std::floor(elapsedTime / (24 * 60 * 60 * 1000.0)), std::fmod(elapsedTime / (60 * 60 * 1000.0), 24));
  else Brain.Screen.printAt(30 - easeS4 * screenWidth, 150, false, "%.0f years %.0f days", std::floor(elapsedTime / (356 * 24 * 60 * 60 * 1000.0)), std::fmod(elapsedTime / (24 * 60 * 60 * 1000.0), 365));

  Brain.Screen.setFont(vex::mono12);
  int textY = 0;
  std::string tempString = auton.endMessage;

  while(tempString.size() > 0 && textY < screenHeight - 70)
  {
    int printLength = std::min(29, (int)tempString.size());
    if(printLength == 29)
    {
      while(true)
      {
        if(tempString[printLength - 1] == ' ') break;
        else printLength--;
        if(printLength == 0) {printLength = 29;break;}
      }
    }
    bool newLine = false;
    for(int i = 0; i < printLength; i++)
    {
      if(tempString[i] == '\n')
      {
        newLine = true;
        printLength = i;
      }
    }
    Brain.Screen.printAt(screenWidth * 0.6 + 10  - easeS4 * screenWidth, 70 + textY, false, tempString.substr(0, printLength).c_str());
    tempString.erase(0, printLength + (newLine ? 1 : 0));
    textY += 12;
  }

  Brain.Screen.render();
}