#include "v5.h"
#include "v5_vcs.h"
#include <vector>
#include <algorithm>

#pragma once

namespace lib81k
{
  class controlsController
  {
    public:
      struct btnData { int s, c; long sTime, cTime; };
      union { btnData all[12]; struct { btnData x, a, b, y, u, r, d, l, l1, l2, r1, r2; }; };
      vex::controller * targetController;

      controlsController(vex::controller & targetController) : targetController(&targetController)
      {
        for(int i = 0; i < 8; i++)
        {
          all[i].s = 2;
          all[i].c = 0;
          all[i].sTime = 0;
          all[i].cTime = 0;
        }
      }

      void doControlCycle();
  };

  class controlsScreen
  {
    public:
      struct { int state, startX, startY, preX, preY, moveX, moveY, totalMovement; };
      vex::brain::lcd * targetScreen;

      controlsScreen(vex::brain::lcd & targetScreen) : targetScreen(&targetScreen) {}

      void doControlCycle();
  };
}