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
	bool:isExtended
}

static arr_weaponStats[Weapon][WeaponStats];

stock Weapon_Define(Weapon:weapon, inherit_id, bool:extended = false) {
	arr_weaponStats[weapon][weaponInheritId] = inherit_id;
	arr_weaponStats[weapon][isExtended] = extended;
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
	Weapon_Define(Colt45, WEAPON_COLT45);
	Weapon_Define(Silenced, WEAPON_SILENCED);
	Weapon_Define(Deagle, WEAPON_DEAGLE);
	Weapon_Define(Shotgun, WEAPON_SHOTGUN);
	Weapon_Define(SawnoffShotgun, WEAPON_SAWEDOFF);
	Weapon_Define(CombatShotgun, WEAPON_SHOTGSPA);
	Weapon_Define(Uzi, WEAPON_UZI);
	Weapon_Define(MP5, WEAPON_MP5);
	Weapon_Define(AK47, WEAPON_AK47);
	Weapon_Define(M4, WEAPON_M4);
	Weapon_Define(Tec9, WEAPON_TEC9);
	Weapon_Define(CountryRifle, WEAPON_RIFLE);
	Weapon_Define(SniperRifle, WEAPON_SNIPER);
	return 1;
}