/// This is where the magic happens; program your plug-in's core behavior here.

#include "game_hooks.h"
#include <graphics/graphics.h>
#include <SCBW/api.h>
#include <SCBW/scbwdata.h>
#include <SCBW/ExtendSightLimit.h>
#include "psi_field.h"
#include <cstdio>


namespace hooks {

/// This hook is called every frame; most of your plugin's logic goes here.
bool nextFrame() {
  if (!scbw::isGamePaused()) { //If the game is not paused
    scbw::setInGameLoopState(true); //Needed for scbw::random() to work
    graphics::resetAllGraphics();
    hooks::updatePsiFieldProviders();
    
    //This block is executed once every game.
    if (*elapsedTimeFrames == 0) {
      //Write your code here
      scbw::printText("SC:X v0.01");
	      //KYSXD Add 2 workers and a Guardian IF it's not an UMS map
	  if (!(*GAME_TYPE == 10)) {
		  for (CUnit* townHall = *firstVisibleUnit; townHall; townHall = townHall->link.next) {
			  if (townHall->mainOrderId != OrderId::Die) {
				  u16 workerUnitId = UnitId::None;
				  if (townHall->id == UnitId::TerranCommandCenter) {
					  workerUnitId = UnitId::TerranSCV;
				  }
				  else if (townHall->id == UnitId::ZergHatchery) {
					  workerUnitId = UnitId::ZergDrone;
				  }
				  else if (townHall->id == UnitId::ProtossNexus) {
					  workerUnitId = UnitId::ProtossProbe;
				  }
				  else
					  continue;
				  scbw::createUnitAtPos(workerUnitId, townHall->playerId, townHall->getX(), townHall->getY());
			  }
		  }
	  }
    }

    //Loop through all visible units in the game.
    for (CUnit *unit = *firstVisibleUnit; unit; unit = unit->link.next) {
      //Write your code here
    }

    scbw::setInGameLoopState(false);
  }
  return true;
}

bool gameOn() {
  return true;
}

bool gameEnd() {
  return true;
}

} //hooks
