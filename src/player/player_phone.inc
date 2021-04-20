#if defined player_phone_included
	#endinput
#endif

#define player_phone_included

CMD:phone(playerid) {
	for(new i = 1, item; i != -1; i--) {
		item = Player_GetHandItemId(playerid, i);
		if(item != -1 && Item_GetAbility(item) == iAbilityPhone) {
			IPhone_InitInterface(item, 320.0, 240.0);
			IPhone_InitFinger(playerid);
			break;
		}
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerUpdate(playerid) {
	if(IPhone_FingerHas(playerid)) {
		new keys,
			ud,
			lr;
		GetPlayerKeys(playerid, keys, ud, lr);
		if(lr == KEY_LEFT) {
			IPhone_FingerMoveLeft(playerid, IPhone_GetBody(Player_GetHandItemId(playerid, 1)));
		} else if(lr == KEY_RIGHT) {
			IPhone_FingerMoveRight(playerid, IPhone_GetBody(Player_GetHandItemId(playerid, 1)));
		} else if(ud == KEY_UP) {
			IPhone_FingerMoveUp(playerid, IPhone_GetBody(Player_GetHandItemId(playerid, 1)));
		} else if(ud == KEY_DOWN) {
			IPhone_FingerMoveDown(playerid, IPhone_GetBody(Player_GetHandItemId(playerid, 1)));
		}
	}
	return 1;
}