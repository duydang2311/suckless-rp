#if defined breed_key_included
	#endinput
#endif

#define breed_key_included

#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(newkeys & KEY_YES) {
		new id = Player_GetCarryingItem(playerid);
		if(id != -1 && Item_GetAbility(id) == iAbilityWhisk) {
			new
				Float:x,
				Float:y,
				Float:z,
				closest_npc = INVALID_PLAYER_ID,
				Float:min_dist = 2.0,
				Float:dist;
			GetPlayerPos(playerid, x, y, z);
			foreach(new i : FCNPC) {
				printf("npc %d, id %d", i, Breed_GetIdByNPCId(i));
				if(Breed_GetIdByNPCId(i) == -1) {
					continue;
				}
				dist = GetPlayerDistanceFromPoint(i, x, y, z);
				if(dist < min_dist) {
					closest_npc = i;
					min_dist = dist;
				}
			}
			printf("closest npc %d", closest_npc);
			if(closest_npc != INVALID_PLAYER_ID) {
				Player_StopCarrying(playerid);
				FCNPC_SetAngleToPlayer(closest_npc, playerid);
				Breed_Eat(Breed_GetIdByNPCId(closest_npc));
				Item_Delete(id);
			}
		}
	}
	return 1;
}