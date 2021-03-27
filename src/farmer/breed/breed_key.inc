#if defined breed_key_included
	#endinput
#endif

#define breed_key_included

#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(newkeys & KEY_YES) {
		new item_id = Player_GetCarryingItem(playerid);
		if(item_id != -1 && Item_GetAbility(item_id) == iAbilityWhisk) {
			new
				Float:x,
				Float:y,
				Float:z,
				closest_npc = INVALID_PLAYER_ID,
				Float:min_dist = 2.0,
				Float:dist,
				breed_id;
			GetPlayerPos(playerid, x, y, z);
			foreach(new i : FCNPC) {
				breed_id = Breed_GetIdByNPCId(i);
				if(breed_id == -1) {
					continue;
				}
				dist = GetPlayerDistanceFromPoint(i, x, y, z);
				if(dist < min_dist && Breed_GetFoodObjectId(breed_id) == -1) {
					closest_npc = i;
					min_dist = dist;
				}
			}
			if(closest_npc != INVALID_PLAYER_ID) {
				Player_StopCarrying(playerid);
				Breed_Eat(Breed_GetIdByNPCId(closest_npc));
				Item_Delete(item_id);
			}
		}
	}
	return 1;
}