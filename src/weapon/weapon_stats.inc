#if defined weapon_stats_included
	#endinput
#endif

#define weapon_stats_included

enum Weapon {
	BrassKnuckle,
	GolfClub,
	NiteStick,
	Knife,
	Bat,
	Shovel,
	PoolStick,
	Katana,
	Chainsaw,
	PurpleDildo,
	Dildo,
	Vibrator,
	SilverVibrator,
	Flower,
	Cane,
	Colt45,
	Silenced,
	Deagle,
	Shotgun,
	SawnoffShotgun,
	CombatShotgun,
	Uzi,
	MP5,
	AK47,
	M4,
	Tec9,
	CountryRifle,
	SniperRifle,
	RPG,
	HSRocket,
	Flamethrower,
	Minigun,
	Spraycan,
	FireExtinguisher,
	Camera,
	Parachute
}

enum WeaponStats {
	weaponInheritId,
	bool:isGun,
	bool:isExtended,
	weaponSlot,
	weaponModel
}

static arr_weaponStats[Weapon][WeaponStats];

stock Weapon_Define(Weapon:weapon, inherit_id, slot, model, bool:gun = false, bool:extended = false) {
	arr_weaponStats[weapon][weaponInheritId] = inherit_id;
	arr_weaponStats[weapon][isExtended] = extended;
	arr_weaponStats[weapon][isGun] = gun;
	arr_weaponStats[weapon][weaponSlot] = slot;
	arr_weaponStats[weapon][weaponModel] = model;
	return 1;
}

stock Weapon_SetPlayerArmed(playerid, Weapon:weapon, ammo) {
	if(arr_weaponStats[weapon][isExtended]) {
		// TODO: attach custom object
	}
	GivePlayerWeapon(playerid, arr_weaponStats[weapon][weaponInheritId], ammo);
	return 1;
}

stock Weapon_GetAmmo(playerid, Weapon:weapon) {
	new
		_weapon,
		ammo;
	GetPlayerWeaponData(playerid, arr_weaponStats[weapon][weaponSlot], _weapon, ammo);
	return ammo;
}

stock Weapon_Reset(playerid) {
	ResetPlayerWeapons(playerid);
	return 1;
}

stock Weapon_GetName(Weapon:weapon, name[], size = sizeof(name)) {
	GetWeaponName(arr_weaponStats[weapon][weaponInheritId], name, size);
	return 1;
}

stock Weapon_GetModel(Weapon:weapon) {
	return arr_weaponStats[weapon][weaponModel];
}

stock Weapon_IsGun(Weapon:weapon) {
	return arr_weaponStats[weapon][isGun];
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	Weapon_Define(BrassKnuckle, WEAPON_BRASSKNUCKLE, 0, 331);
	Weapon_Define(GolfClub, WEAPON_GOLFCLUB, 1, 333);
	Weapon_Define(NiteStick, WEAPON_NITESTICK, 1, 334);
	Weapon_Define(Knife, WEAPON_KNIFE, 1, 335);
	Weapon_Define(Bat, WEAPON_BAT, 1, 336);
	Weapon_Define(Shovel, WEAPON_SHOVEL, 1, 337);
	Weapon_Define(PoolStick, WEAPON_POOLSTICK, 1, 338);
	Weapon_Define(Katana, WEAPON_KATANA, 1, 339);
	Weapon_Define(Chainsaw, WEAPON_CHAINSAW, 1, 341);
	Weapon_Define(PurpleDildo, WEAPON_DILDO, 10, 321);
	Weapon_Define(Dildo, WEAPON_DILDO2, 10, 322);
	Weapon_Define(Vibrator, WEAPON_VIBRATOR, 10, 323);
	Weapon_Define(SilverVibrator, WEAPON_VIBRATOR2, 10, 324);
	Weapon_Define(Flower, WEAPON_FLOWER, 10, 325);
	Weapon_Define(Cane, WEAPON_CANE, 10, 342);
	Weapon_Define(Colt45, WEAPON_COLT45, 2, 346, true);
	Weapon_Define(Silenced, WEAPON_SILENCED, 2, 347, true);
	Weapon_Define(Deagle, WEAPON_DEAGLE, 2, 348, true);
	Weapon_Define(Shotgun, WEAPON_SHOTGUN, 3, 349, true);
	Weapon_Define(SawnoffShotgun, WEAPON_SAWEDOFF, 3, 350, true);
	Weapon_Define(CombatShotgun, WEAPON_SHOTGSPA, 3, 351, true);
	Weapon_Define(Uzi, WEAPON_UZI, 4, 352, true);
	Weapon_Define(MP5, WEAPON_MP5, 4, 353, true);
	Weapon_Define(AK47, WEAPON_AK47, 5, 355, true);
	Weapon_Define(M4, WEAPON_M4, 5, 356, true);
	Weapon_Define(Tec9, WEAPON_TEC9, 4, 372, true);
	Weapon_Define(CountryRifle, WEAPON_RIFLE, 6, 357, true);
	Weapon_Define(SniperRifle, WEAPON_SNIPER, 6, 358, true);
	Weapon_Define(RPG, WEAPON_ROCKETLAUNCHER, 7, 359, true);
	Weapon_Define(HSRocket, WEAPON_HEATSEEKER, 7, 360, true);
	Weapon_Define(Flamethrower, WEAPON_FLAMETHROWER, 7, 361, true);
	Weapon_Define(Minigun, WEAPON_MINIGUN, 7, 362, true);
	Weapon_Define(Spraycan, WEAPON_SPRAYCAN, 9, 365, true);
	Weapon_Define(FireExtinguisher, WEAPON_FIREEXTINGUISHER, 9, 366, true);
	Weapon_Define(Camera, WEAPON_CAMERA, 9, 367, true);
	Weapon_Define(Parachute, WEAPON_PARACHUTE, 11, 371);
	return 1;
}