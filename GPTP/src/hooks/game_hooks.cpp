/// This is where the magic happens; program your plug-in's core behavior here.

#include "game_hooks.h"
#include <graphics/graphics.h>
#include <SCBW/api.h>
#include <SCBW/scbwdata.h>
#include <SCBW/ExtendSightLimit.h>
#include "psi_field.h"
#include <cstdio>
#include <AI/ai_common.h>
#include "rally_point.h"


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
      scbw::printText(PLUGIN_NAME);
	      //KYSXD Add 12 workers
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
				  CUnit *target = NULL;
				  u16 range = townHall->getSightRange();
				  if (townHall->id == UnitId::command_center
					  || townHall->id == UnitId::hatchery
					  || townHall->id == UnitId::nexus) {
						  target = scbw::UnitFinder::getNearestTarget(townHall, [] (CUnit *tunit) -> bool {
							  if (tunit->id == UnitId::mineral_field_1
								  || tunit->id == UnitId::mineral_field_2
								  || tunit->id == UnitId::mineral_field_3){
									  return true;
							  }
							  else return false;
						  }
						  );
						  if (target != NULL) {
							  townHall->rally.pt.x = target->getX();
							  townHall->rally.pt.y = target->getY();
						  }
						  for(CUnit *worker = *firstVisibleUnit; worker; worker = worker->link.next) {
							  if((worker->id == workerUnitId)
								  && worker->playerId == townHall->playerId) {
									  worker->orderTo(OrderId::Harvest1, target);
							  }
						  }
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
