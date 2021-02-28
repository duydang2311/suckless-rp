#if defined player_stats_included
	#endinput
#endif

#define player_stats_included

enum PlayerStats {
	Float:pHealth,
	Float:pArmour
}

#include <YSI_Coding\y_hooks>

hook Player_OnSpawn(playerid) {

	return 1;
}