#if defined player_hand_key_included
	#endinput
#endif

#define player_hand_key_included

#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(newkeys & KEY_NO && !IsPlayerInAnyVehicle(playerid)) {
		for(new i = 1, id; i != -1; i--) {
			id = Player_GetHandItemId(playerid, i);
			if(id != -1) {
				new
					Float:x,
					Float:y,
					Float:angle;
				GetPlayerPos(playerid, x, y, angle);
				GetPlayerFacingAngle(playerid, angle);
				Player_UnequipItem(playerid, i);
				IGround_CAUpdate(id, x, y, angle, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
				return Y_HOOKS_BREAK_RETURN_1;
			}
		}
	}
	return 1;
}