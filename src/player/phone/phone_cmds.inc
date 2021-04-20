#if defined player_phone_cmds_included
	#endinput
#endif

#define player_phone_cmds_included

static Map:m_phoneItemId;

CMD:phone(playerid) {
	for(new i = 1, item; i != -1; i--) {
		item = Player_GetHandItemId(playerid, i);
		if(item != -1 && IPhone_Has(item)) {
			IPhone_ShowUIForPlayer(item, playerid, 320.0, 240.0);
			IPFinger_Init(playerid, IPhone_GetModel(item));
			if(!map_valid(m_phoneItemId)) {
				m_phoneItemId = map_new();
			}
			map_set(m_phoneItemId, playerid, item);
			break;
		}
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerUpdate(playerid) {
	if(map_valid(m_phoneItemId) && map_has_key(m_phoneItemId, playerid)) {
		new item = map_get(m_phoneItemId, playerid);
		if(!IPhone_Has(item)) {
			map_remove(m_phoneItemId, playerid);
			if(!map_size(m_phoneItemId)) {
				map_delete(m_phoneItemId);
				m_phoneItemId = Map:-1;
			}
			return 1;
		}

		new keys,
			ud,
			lr;
		GetPlayerKeys(playerid, keys, ud, lr);
		if(lr == KEY_LEFT) {
			IPFinger_MoveLeft(playerid, IPhone_GetModel(item));
		} else if(lr == KEY_RIGHT) {
			IPFinger_MoveRight(playerid, IPhone_GetModel(item));
		} else if(ud == KEY_UP) {
			IPFinger_MoveUp(playerid, IPhone_GetModel(item));
		} else if(ud == KEY_DOWN) {
			IPFinger_MoveDown(playerid, IPhone_GetModel(item));
		}
	}
	return 1;
}