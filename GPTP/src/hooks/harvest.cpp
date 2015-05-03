#include "harvest.h"
#include "../SCBW/enumerations.h"
#include "../SCBW/api.h"

//Helper functions
void updateMineralPatchImage(CUnit *mineralPatch);
void setResourceAmountCarried(CUnit *worker, u8 amountCarried, u32 chunkImageId, bool isMineral);
int min_amount = 5;
int gas_amount = 4;
int rmin_amount = 7;
int rgas_amount = 6;
int dep_amount = 0;

//-------- Actual hooks --------//

namespace hooks {

//Harvests minerals/gas from the @p resource and returns the amount that a
//worker should carry.
u8 harvestResourceFrom(CUnit *resource, bool isMineral) {
	int harvest_amount = 0;
  //Default StarCraft behavior
	if (isMineral) {
		harvest_amount = min_amount;
	}
	else {harvest_amount = gas_amount;
	}
	if (resource->building.resource.resourceAmount < harvest_amount) {
		if (isMineral) {
			resource->remove();
			return (u8) resource->building.resource.resourceAmount;
		}
		else {
			resource->building.resource.resourceAmount = 0;
			return dep_amount;
		}
	}
	else {
		resource->building.resource.resourceAmount -= harvest_amount;
    
		if (isMineral) {
			if (resource->building.resource.resourceAmount > 0)
				updateMineralPatchImage(resource);
			else
				resource->remove();
		}
		else if (resource->building.resource.resourceAmount < harvest_amount)
			scbw::showErrorMessageWithSfx(resource->playerId, 875, 20); //Gas depleted message and sound
		return harvest_amount;
	}
}

//Transfers a set amount of resources from a resource patch to a worker.
void transferResourceToWorkerHook(CUnit *worker, CUnit *resource) {
  //Default StarCraft behavior

  u32 chunkImageId;
  bool isMineral = false;

  if (176 <= resource->id && resource->id <= 178) {  //Is a mineral patch
    chunkImageId = ImageId::MineralChunkType1;
    isMineral = true;
  }
  else if (resource->id == UnitId::assimilator)
    chunkImageId = ImageId::ProtossGasOrbType1;
  else if (resource->id == UnitId::extractor)
    chunkImageId = ImageId::ZergGasSacType1;
  else if (resource->id == UnitId::refinery)
    chunkImageId = ImageId::TerranGasTankType1;
  else
    return;

  u8 resourceAmount = harvestResourceFrom(resource, isMineral);
  if (resourceAmount < min_amount)
    chunkImageId += 1;  //Use depleted (smaller) chunk image

  if (resourceAmount > 0) {
    if (worker->resourceType & 3) { //Is carrying a mineral / gas
      //Identical to function @ 0x004F3900
      //Remove powerup shadows (mineral chunk shadow, psi emitter shadow, etc.)
      worker->sprite->removeOverlay(ImageId::MineralChunkShadow, ImageId::PsiEmitterShadow_Carried);
      //Remove powerup graphics
      worker->sprite->removeOverlay(ImageId::Flag, ImageId::TerranGasTankType2);
      //Remove Uraj / Khalis
      worker->sprite->removeOverlay(ImageId::Uraj, ImageId::Khalis);
      worker->resourceType = 0;
    }

    setResourceAmountCarried(worker, resourceAmount, chunkImageId, isMineral);
  }
}

} //hooks

//-------- Helper function definitions. Do NOT change this! --------//

//Identical to function @ 0x00468830
void updateMineralPatchImage(CUnit *mineralPatch) {
  IscriptAnimation::Enum anim;

  if (mineralPatch->building.resource.resourceAmount >= 750)
    anim = IscriptAnimation::WorkingToIdle;
  else if (mineralPatch->building.resource.resourceAmount >= 500)
    anim = IscriptAnimation::AlmostBuilt;
  else if (mineralPatch->building.resource.resourceAmount >= 250)
    anim = IscriptAnimation::SpecialState2;
  else
    anim = IscriptAnimation::SpecialState1;

  if (anim != mineralPatch->building.resource.resourceIscript) {
    mineralPatch->building.resource.resourceIscript = anim;
    mineralPatch->sprite->playIscriptAnim(anim);
  }
}

void updateImagePositionOffset(CImage *image) {
  const u32 Func_updateImagePositionOffset = 0x004D5A00;
  __asm {
    PUSHAD
    MOV ECX, image
    CALL Func_updateImagePositionOffset
    POPAD
  }
}

//Identical to function @ 0x004F3AF0
void setResourceAmountCarried(CUnit *worker, u8 amountCarried, u32 chunkImageId, bool isMineral) {
  if (worker->resourceType) return;
  worker->resourceType = isMineral ? 2 : 1;
  
  CImage *chunkImage = worker->sprite->createOverlay(chunkImageId);
  if (chunkImage && !(chunkImage->flags & 0x80)) {
    chunkImage->flags |= 0x80;
    updateImagePositionOffset(chunkImage);
  }

  worker->worker.resourceCarryAmount = amountCarried;
  scbw::refreshConsole();
}
