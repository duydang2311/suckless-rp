#if defined player_hand_key_included
	#endinput
#endif

#define player_hand_key_included

#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(!IsPlayerInAnyVehicle(playerid)) {
		if(newkeys & KEY_NO) {
			new id = Player_GetCarryingItem(playerid);
			if(id != -1) {
				new
					Float:x,
					Float:y,
					Float:angle;
				GetPlayerPos(playerid, x, y, angle);
				GetPlayerFacingAngle(playerid, angle);
				Player_StopCarrying(playerid);
				IGround_CAUpdate(id, x, y, angle, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid), 1.5);
				return Y_HOOKS_BREAK_RETURN_1;
			}
			for(new i = 1; i != -1; i--) {
				id = Player_GetHandItemId(playerid, i);
				if(id != -1) {
					new
						Float:x,
						Float:y,
						Float:angle;
					GetPlayerPos(playerid, x, y, angle);
					GetPlayerFacingAngle(playerid, angle);
					Player_UnequipItem(playerid, i);
					IGround_CAUpdate(id, x, y, angle, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid), 1.5);
					return Y_HOOKS_BREAK_RETURN_1;
				}
			}
		}
		if(newkeys & KEY_YES) {
			new hand = Player_GetFreeHand(playerid);
			if(hand == -1 || Player_GetCarryingItem(playerid) != -1) {
				return 1;
			}
			new
				Float:x,
				Float:y,
				Float:z,
				items[10],
				count;
			GetPlayerPos(playerid, x, y, z);
			count = Streamer_GetNearbyItems(x, y, z, STREAMER_TYPE_OBJECT, items, sizeof(items), 2.0, GetPlayerVirtualWorld(playerid));
			count = (count > sizeof(items)) ? sizeof(items) : count;
			for(new i = 0; i != count; i++) {
				if(Streamer_GetIntData(STREAMER_TYPE_OBJECT, items[i], E_STREAMER_EXTRA_ID) == IGround_GetStreamerExtraId()) {
					new id = IGround_GetByObjectId(items[i]);
					if(id != -1) {
						if(Item_GetOccupiedSpace(id) > 1) {
							if(Player_GetHandItemId(playerid, 0) != -1 || Player_GetHandItemId(playerid, 1) != -1) {
								continue;
							}
							new slot = Player_GetFreeAttachmentSlot(playerid);
							if(slot == -1) {
								Player_Msg(playerid, COLOR_FADE2, "Ban khong con slot gan vat pham, hay an bot vat pham tren nguoi truoc.");
								return Y_HOOKS_BREAK_RETURN_1;
							}
							Player_CarryItem(playerid, id, slot);
						} else {
							Player_EquipItem(playerid, hand, id);
						}
						IGround_Delete(id);
						return Y_HOOKS_BREAK_RETURN_1;
					}
				}
			}
		}
	}
	return 1;
}