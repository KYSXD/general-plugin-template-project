#include "../SCBW/api.h"
#include "../SCBW/enumerations/WeaponId.h"
#include <cstdio>


char buffer[128];

//Returns the special damage multiplier factor for units that don't use the
//"Damage Factor" property in weapons.dat.
u8 getDamageFactorForTooltip(u8 weaponId, const CUnit *unit) {
  //Default StarCraft behavior
  if (unit->id == UnitId::firebat || unit->id == UnitId::gui_montag
      || unit->id == UnitId::zealot || unit->id == UnitId::fenix_zealot)
    return 2;

  if (unit->id == UnitId::valkyrie)
    return 1;

  return Weapon::DamageFactor[weaponId];
}

//Returns the C-string for the tooltip text of the unit's weapon icon.
//This function is used for weapon icons and special icons.
//Precondition: @p entryStrIndex is a stat_txt.tbl string index.
const char* getWeaponTooltipString(u8 weaponId, const CUnit *unit, u16 entryStrIndex) {
  //Default StarCraft behavior

  const char *entryName = scbw::getStatTxtTblString(entryStrIndex);
  const char *damageStr = scbw::getStatTxtTblString(777);         //"Damage:"

  const u8 damageFactor = getDamageFactorForTooltip(weaponId, unit);
  const u8 upgradeLevel = scbw::getUpgradeLevel(unit->playerId, Weapon::DamageUpgrade[weaponId]);
  const u16 baseDamage = Weapon::DamageAmount[weaponId] * damageFactor;
  const u16 bonusDamage = Weapon::DamageBonus[weaponId] * damageFactor * upgradeLevel;

  if (weaponId == WeaponId::HaloRockets) {
    if (bonusDamage > 0) {
      const char *perRocketStr = scbw::getStatTxtTblString(1301); //"per rocket"
      sprintf_s(buffer, sizeof(buffer), "%s\n%s %d+%d %s",
                entryName, damageStr, baseDamage, bonusDamage, perRocketStr);
    }
    else
      sprintf_s(buffer, sizeof(buffer), "%s\n%s %d %s",
                entryName, damageStr, baseDamage, bonusDamage);
  }
  else {
    if (bonusDamage > 0)
      sprintf_s(buffer, sizeof(buffer), "%s\n%s %d+%d",
                entryName, damageStr, baseDamage, bonusDamage);
    else
      sprintf_s(buffer, sizeof(buffer), "%s\n%s %d",
                entryName, damageStr, baseDamage);
  }

  return buffer;
}

namespace hooks {

//Returns the C-string for the tooltip text of the unit's weapon icon.
const char* getWeaponTooltipString(u8 weaponId, const CUnit *unit) {
  return getWeaponTooltipString(weaponId, unit, Weapon::Label[weaponId]);
}

//Returns the C-string for the tooltip text of the unit's armor icon.
const char* getArmorTooltipString(const CUnit *unit) {
  //Default StarCraft behavior
  
  const u16 labelId = Upgrade::Label[Unit::ArmorUpgrade[unit->id]];
  const char *armorUpgradeName = scbw::getStatTxtTblString(labelId);
  const char *armorStr = scbw::getStatTxtTblString(778);          //"Armor:"

  const u8 baseArmor = Unit::ArmorAmount[unit->id];
  const u8 bonusArmor = unit->getArmorBonus();

  if (bonusArmor > 0)
    sprintf_s(buffer, sizeof(buffer), "%s\n%s %d+%d",
              armorUpgradeName, armorStr, baseArmor, bonusArmor);
  else
    sprintf_s(buffer, sizeof(buffer), "%s\n%s %d",
              armorUpgradeName, armorStr, baseArmor);

  return buffer;
}


//Returns the C-string for the tooltip text of the plasma shield icon.
const char* getShieldTooltipString(const CUnit *unit) {
  //Default StarCraft behavior

  const u16 labelId = Upgrade::Label[UpgradeId::ProtossPlasmaShields];
  const char *shieldUpgradeName = scbw::getStatTxtTblString(labelId);
  const char *shieldStr = scbw::getStatTxtTblString(779);         //"Shields:"

  const u8 shieldUpgradeLevel = scbw::getUpgradeLevel(unit->playerId, UpgradeId::ProtossPlasmaShields);

  if (shieldUpgradeLevel > 0)
    sprintf_s(buffer, sizeof(buffer), "%s\n%s %d+%d",
              shieldUpgradeName, shieldStr, 0, shieldUpgradeLevel);
  else
    sprintf_s(buffer, sizeof(buffer), "%s\n%s %d",
              shieldUpgradeName, shieldStr, 0);

  return buffer;
}

//Returns the C-string for the tooltip text of the Interceptor icon (Carriers),
//Scarab icon (Reavers), Nuclear Missile icon (Nuclear Silos), and Spider Mine
//icon (Vultures).
const char* getSpecialTooltipString(u16 iconUnitId, const CUnit *unit) {
  //Default StarCraft behavior

  if (iconUnitId == UnitId::interceptor) {
    return getWeaponTooltipString(WeaponId::PulseCannon, unit, 791);  //"Interceptors"
  }

  if (iconUnitId == UnitId::scarab) {
    return getWeaponTooltipString(WeaponId::Scarab, unit, 792);       //"Scarabs"
  }

  if (iconUnitId == UnitId::nuclear_missile) {
    return scbw::getStatTxtTblString(793);  //"Nukes"
  }

  if (iconUnitId == UnitId::spider_mine) {
    return getWeaponTooltipString(WeaponId::SpiderMines, unit, 794);  //"Spider Mines"
  }

  //Should never reach here
  return "";
}

} //hooks

