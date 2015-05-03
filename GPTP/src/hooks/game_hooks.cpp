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
	      //KYSXD Add 1 workers
	  if (!(*GAME_TYPE == 10)) {
		  u16 initialworkeramount = 12;
		  for (CUnit* townHall = *firstVisibleUnit; townHall; townHall = townHall->link.next) {
			  if (townHall->mainOrderId != OrderId::Die) {
				  u16 workerUnitId = UnitId::None;
				  if (townHall->id == UnitId::command_center) {
					  workerUnitId = UnitId::scv;
				  }
				  else if (townHall->id == UnitId::hatchery) {
					  workerUnitId = UnitId::drone;
				  }
				  else if (townHall->id == UnitId::nexus) {
					  workerUnitId = UnitId::probe;
				  }
				  else
					  continue;
				  for (int i=0; i<(initialworkeramount-4); i++) {
					  scbw::createUnitAtPos(workerUnitId, townHall->playerId, townHall->getX(), townHall->getY());
				  }
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
