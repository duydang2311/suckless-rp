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
	bool:isExtended
}

static arr_weaponStats[Weapon][WeaponStats];

stock Weapon_Define(Weapon:weapon, inherit_id, bool:gun = false, bool:extended = false) {
	arr_weaponStats[weapon][weaponInheritId] = inherit_id;
	arr_weaponStats[weapon][isExtended] = extended;
	arr_weaponStats[weapon][isGun] = gun;
	return 1;
}

stock Weapon_SetPlayerArmed(playerid, Weapon:weapon, ammo) {
	if(arr_weaponStats[weapon][isExtended]) {
		// TODO: attach custom object
	}
	GivePlayerWeapon(playerid, arr_weaponStats[weapon][weaponInheritId], ammo);
	return 1;
}

stock Weapon_Reset(playerid) {
	ResetPlayerWeapons(playerid);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	Weapon_Define(BrassKnuckle, WEAPON_BRASSKNUCKLE);
	Weapon_Define(GolfClub, WEAPON_GOLFCLUB);
	Weapon_Define(Knife, WEAPON_KNIFE);
	Weapon_Define(Bat, WEAPON_BAT);
	Weapon_Define(Shovel, WEAPON_SHOVEL);
	Weapon_Define(PoolStick, WEAPON_POOLSTICK);
	Weapon_Define(Katana, WEAPON_KATANA);
	Weapon_Define(PurpleDildo, WEAPON_DILDO);
	Weapon_Define(Dildo, WEAPON_DILDO2);
	Weapon_Define(Vibrator, WEAPON_VIBRATOR);
	Weapon_Define(SilverVibrator, WEAPON_VIBRATOR2);
	Weapon_Define(Flower, WEAPON_FLOWER);
	Weapon_Define(Cane, WEAPON_CANE);
	Weapon_Define(Colt45, WEAPON_COLT45, true);
	Weapon_Define(Silenced, WEAPON_SILENCED, true);
	Weapon_Define(Deagle, WEAPON_DEAGLE, true);
	Weapon_Define(Shotgun, WEAPON_SHOTGUN, true);
	Weapon_Define(SawnoffShotgun, WEAPON_SAWEDOFF, true);
	Weapon_Define(CombatShotgun, WEAPON_SHOTGSPA, true);
	Weapon_Define(Uzi, WEAPON_UZI, true);
	Weapon_Define(MP5, WEAPON_MP5, true);
	Weapon_Define(AK47, WEAPON_AK47, true);
	Weapon_Define(M4, WEAPON_M4, true);
	Weapon_Define(Tec9, WEAPON_TEC9, true);
	Weapon_Define(CountryRifle, WEAPON_RIFLE, true);
	Weapon_Define(SniperRifle, WEAPON_SNIPER, true);
	Weapon_Define(RPG, WEAPON_ROCKETLAUNCHER, true);
	Weapon_Define(HSRocket, WEAPON_HEATSEEKER, true);
	Weapon_Define(Flamethrower, WEAPON_FLAMETHROWER, true);
	Weapon_Define(Minigun, WEAPON_MINIGUN, true);
	Weapon_Define(Spraycan, WEAPON_SPRAYCAN, true);
	Weapon_Define(FireExtinguisher, WEAPON_FIREEXTINGUISHER, true);
	Weapon_Define(Camera, WEAPON_CAMERA, true);
	Weapon_Define(Parachute, WEAPON_PARACHUTE, true);
	return 1;
}