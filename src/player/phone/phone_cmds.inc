#if defined player_phone_cmds_included
	#endinput
#endif

#define player_phone_cmds_included

static Map:m_phoneItemId;

CMD:phone(playerid) {
	for(new i = 1, item; i != -1; i--) {
		item = Player_GetHandItemId(playerid, i);
		if(item != -1 && IPhone_Has(item)) {
			// TogglePlayerControllable(playerid, false);
			IPhone_ShowUIForPlayer(item, playerid, RandomFloat(200.0, 470.0), RandomFloat(150.0, 230.0));
			task_unblock(1);
			await IPApp_InitStatsAsync(item, AppMenu);
			IPApp_ShowUIForPlayer(item, AppMenu, playerid);
			// IPFinger_Init(playerid, item);
			SelectTextDraw(playerid, 0x115599FF);
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
		static last = 0;
		if((GetTickCount() - last) < 100) {
			return 1;
		}

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
			IPFinger_MoveLeft(playerid, item);
		} else if(lr == KEY_RIGHT) {
			IPFinger_MoveRight(playerid, item);
		}
		if(ud == KEY_UP) {
			IPFinger_MoveUp(playerid, item);
		} else if(ud == KEY_DOWN) {
			IPFinger_MoveDown(playerid, item);
		}
		last = GetTickCount();
	}
	return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(map_valid(m_phoneItemId) && map_has_key(m_phoneItemId, playerid)) {
		new id = map_get(m_phoneItemId, playerid);
		if(newkeys & KEY_SPRINT) { // Space -> select
			printf("current finger index : %d", IPFinger_GetCurrentIndex(playerid, id));
			return Y_HOOKS_BREAK_RETURN_1;
		} else if(newkeys & KEY_SECONDARY_ATTACK) { // Enter -> cancel
			new bool:active[App] = {false, ...};
			IPApp_GetActiveApps(id, active);
			for(new i = 0; i != sizeof(active); i++) {
				if(active[i]) {
					IPApp_TerminateStats(id, i);
				}
			}
			IPApp_HideUIForPlayer(id, playerid);
			IPhone_HideUIForPlayer(id, playerid);
			IPFinger_Hide(playerid, id);
			TogglePlayerControllable(playerid, true);
			map_remove(m_phoneItemId, playerid);
			if(!map_size(m_phoneItemId)) {
				map_delete(m_phoneItemId);
				m_phoneItemId = Map:-1;
			}
			return Y_HOOKS_BREAK_RETURN_1;
		}
	}
	return 1;
}