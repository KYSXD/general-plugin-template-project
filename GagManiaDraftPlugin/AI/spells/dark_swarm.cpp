#include "psi_storm.h"

namespace AI {

class DarkSwarmTargetFinderProc: public scbw::UnitFinderCallbackMatchInterface {
  private:
    const CUnit *caster;
    bool isUnderAttack;
  public:
    DarkSwarmTargetFinderProc(const CUnit *caster, bool isUnderAttack)
      : caster(caster), isUnderAttack(isUnderAttack) {}

    bool match(const CUnit *target) {
      if (target == caster)
        return false;

      if (!isTargetWorthHitting(target, caster))
        return false;

      CUnit *targetOfTarget = target->orderTarget.unit;
      if (!targetOfTarget || targetOfTarget->playerId >= 8)
        return false;

      if (!scbw::isAlliedTo(caster->playerId, targetOfTarget->getLastOwnerId()))
        return false;

      if (targetOfTarget->subunit)
        return false;
      
      if (targetOfTarget->status & UnitStatus::InAir)
        return false;

      u8 totGroundWeapon = targetOfTarget->getActiveGroundWeapon();
      if (totGroundWeapon == WeaponId::None)
        return false;

      if (Weapon::Behavior[totGroundWeapon] != WeaponBehavior::AppearOnAttacker)
        return false;

      if (target->subunit && (Unit::BaseProperty[target->subunit->id] & UnitProperty::Subunit))
        target = target->subunit;

      u8 targetGroundWeapon = target->getActiveGroundWeapon();

      if (targetGroundWeapon == WeaponId::None)
        return false;

      switch (Weapon::Behavior[targetGroundWeapon]) {
        case WeaponBehavior::Fly_DoNotFollowTarget:
        case WeaponBehavior::Fly_FollowTarget:
        case WeaponBehavior::AppearOnTargetUnit:
        case WeaponBehavior::AppearOnTargetSite:
        case WeaponBehavior::Bounce:
          if (!(targetOfTarget->status & UnitStatus::InAir)
              && !(Unit::BaseProperty[targetOfTarget->id] & UnitProperty::Building)
              && scbw::isUnderDarkSwarm(targetOfTarget))
            return true;

          if (scbw::getActiveTileAt(targetOfTarget->getX(), targetOfTarget->getY()).hasDoodadCover)
            return true;

          if (!(targetOfTarget->status & UnitStatus::InAir)
              && !(target->status & UnitStatus::InAir)) {
            u32 targGndHeight = scbw::getGroundHeightAtPos(target->getX(), target->getY());
            u32 totGndHeight = scbw::getGroundHeightAtPos(targetOfTarget->getX(), targetOfTarget->getY());
            if (targGndHeight < totGndHeight)
              return true;
          }
      }

      return false;
    }
};

CUnit* findBestDarkSwarmTarget(const CUnit *caster, bool isUnderAttack) {
  int bounds;
  if (isUnderAttack)
    bounds = 32 * 9;
  else if (isUmsMode(caster->playerId))
    bounds = 32 * 64;
  else
    bounds = 32 * 32;

  CUnit *result = scbw::UnitFinder::getNearest(caster->getX(), caster->getY(),
    caster->getX() - bounds, caster->getY() - bounds,
    caster->getX() + bounds, caster->getY() + bounds,
    DarkSwarmTargetFinderProc(caster, isUnderAttack));

  if (result && (result->status & UnitStatus::InAir))
    result = result->orderTarget.unit;

  return result;
}

} //AI
