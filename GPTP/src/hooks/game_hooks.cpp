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
							  townHall->rally.unit = target;
							  townHall->rally.pt.x = target->getX();
							  townHall->rally.pt.y = target->getY();
							  townHall->moveTarget.unit = target;
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
		if (units_dat::BaseProperty[unit->id] & UnitProperty::ResourceDepot
			&& unit->playerId == *LOCAL_HUMAN_ID) {
				int distance = unit->getSightRange()*32;
				int workers = 0;
				int minerals = 0;
				int gworkers = 0;
				for (CUnit *mineral = *firstVisibleUnit; mineral; mineral = mineral->link.next) {
					if ((176 <= mineral->id && mineral->id <= 178)
						&& scbw::getDistanceFast(unit->getX(), unit->getY(), mineral->getX(), mineral->getY()) < distance) {
							++minerals;
					}
				}
				for (CUnit *gas = *firstVisibleUnit; gas; gas = gas->link.next) {
					if ((gas->id == UnitId::refinery || gas->id == UnitId::assimilator || gas->id == UnitId::extractor)
						&& gas->status & UnitStatus::Completed
						&& scbw::getDistanceFast(unit->getX(), unit->getY(), gas->getX(), gas->getY()) < distance) {
							for (CUnit *gworker = firstPlayerUnit->unit[unit->playerId]; gworker; gworker = gworker->player_link.next) {
								if (gworker->moveTarget.unit != NULL) {
									if (gworker->playerId == *LOCAL_HUMAN_ID
										&& units_dat::BaseProperty[gworker->id] & UnitProperty::Worker
										&& (gworker->mainOrderId == OrderId::HarvestGas1
										|| gworker->mainOrderId == OrderId::HarvestGas2
										|| gworker->mainOrderId == OrderId::HarvestGas3
										|| gworker->mainOrderId == OrderId::Harvest1
										|| gworker->mainOrderId == OrderId::Harvest2
										|| gworker->mainOrderId == OrderId::Harvest3
										|| gworker->mainOrderId == OrderId::Harvest4
										|| gworker->mainOrderId == OrderId::Harvest5
										|| gworker->mainOrderId == OrderId::ReturnGas
										|| gworker->mainOrderId == OrderId::ResetCollision2)
										&& (gworker->moveTarget.unit == unit
										|| gworker->moveTarget.unit == gas)) {
											++gworkers;
									}
								}
							}
							char gasCount[64];
							char gasWorkers[64];
							sprintf_s(gasCount, "\x04 Workers: %d \x01", gworkers);
							sprintf_s(gasWorkers, "\x04 / %d \x01", 3);
							std::string gmessage = std::string(gasCount)+std::string(gasWorkers);
							graphics::drawText(gas->getX()-50, gas->getY()-60, gmessage, graphics::FONT_MEDIUM, graphics::ON_MAP);
					}
				}
				for (CUnit *worker = *firstVisibleUnit; worker; worker = worker->link.next) {
					if (worker->moveTarget.unit != NULL) {
						if (worker->playerId == *LOCAL_HUMAN_ID
							&& units_dat::BaseProperty[worker->id] & UnitProperty::Worker
							&& (worker->mainOrderId == OrderId::Harvest1
							|| worker->mainOrderId == OrderId::Harvest2
							|| worker->mainOrderId == OrderId::Harvest3
							|| worker->mainOrderId == OrderId::Harvest4
							|| worker->mainOrderId == OrderId::Harvest5
							|| worker->mainOrderId == OrderId::MoveToMinerals
							|| worker->mainOrderId == OrderId::MiningMinerals
							|| worker->mainOrderId == OrderId::ReturnMinerals
							|| worker->mainOrderId == OrderId::ResetCollision2
							|| worker->mainOrderId == OrderId::HarvestMinerals2)
							&& (worker->moveTarget.unit == unit
							|| ((176 <= worker->moveTarget.unit->id && worker->moveTarget.unit->id <= 178)
							&& scbw::getDistanceFast(unit->getX(), unit->getY(), worker->moveTarget.unit->getX(), worker->moveTarget.unit->getY()) < distance))) {
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
