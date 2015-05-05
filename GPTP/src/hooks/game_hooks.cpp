/// This is where the magic happens; program your plug-in's core behavior here.

#include "game_hooks.h"
#include <graphics/graphics.h>
#include <SCBW/api.h>
#include <SCBW/scbwdata.h>
#include <SCBW/ExtendSightLimit.h>
#include "psi_field.h"
#include <cstdio>

#include <SCBW/UnitFinder.h>


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
      scbw::printText(PLUGIN_NAME ": Hello, world!");
    }

    //Loop through all visible units in the game.
    for (CUnit *unit = *firstVisibleUnit; unit; unit = unit->link.next) {
      //Write your code here
		if ((unit->id == UnitId::nexus
			|| unit->id == UnitId::command_center
			|| unit->id == UnitId::hatchery
			|| unit->id == UnitId::hive
			|| unit->id == UnitId::lair)
			&& unit->playerId == *LOCAL_HUMAN_ID) {
				int distance = 320;
				int workers = 0;
				int minerals = 0;
				for (CUnit *mineral = *firstVisibleUnit; mineral; mineral = mineral->link.next) {
					if ((mineral->id == UnitId::mineral_field_1
						|| mineral->id == UnitId::mineral_field_2
						|| mineral->id == UnitId::mineral_field_3)
						&& scbw::getDistanceFast(unit->getX(), unit->getY(), mineral->getX(), mineral->getY()) < distance) {
							++minerals;
					}
				}
				for (CUnit *worker = *firstVisibleUnit; worker; worker = worker->link.next) {
					if (worker->moveTarget.unit != NULL) {
						if (worker->playerId == *LOCAL_HUMAN_ID
							&& units_dat::BaseProperty[worker->id] & UnitProperty::Worker
							&& (worker->moveTarget.unit == unit
							|| ((worker->moveTarget.unit->id == UnitId::mineral_field_1
							|| worker->moveTarget.unit->id == UnitId::mineral_field_2
							|| worker->moveTarget.unit->id == UnitId::mineral_field_3) && scbw::getDistanceFast(unit->getX(), unit->getY(), worker->moveTarget.unit->getX(), worker->moveTarget.unit->getY()) < distance))) {
								++workers;
						}
					}
				}
				char workerCount[64];
				char mineralCount[64];
				sprintf_s(workerCount, "\x04 Workers: %d \x01", workers);
				sprintf_s(mineralCount, "\x04 / %d \x01", minerals*3);
				std::string message = std::string(workerCount)+std::string(mineralCount);
				graphics::drawText(unit->getX()-50, unit->getY()-60, message, graphics::FONT_MEDIUM, graphics::ON_MAP);
				graphics::drawCircle(unit->getX(), unit->getY(), distance, graphics::BLUE, graphics::ON_MAP);
		}
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
