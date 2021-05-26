#if defined phone_ui_included
	#endinput
#endif

#define phone_ui_included

static Map:m_playerPhone;

stock IPUI_GetPlayerPhone(playerid) {
	if(!map_valid(m_playerPhone) || !map_has_key(m_playerPhone, playerid)) {
		return -1;
	}
	return map_get(m_playerPhone, playerid);
}

stock IPUI_ShowForPlayer(id, playerid, Float:x, Float:y) {
	if(!IPhone_Has(id)) {
		return 0;
	}

	if(!map_valid(m_playerPhone)) {
		m_playerPhone = map_new();
	}
	map_set(m_playerPhone, playerid, id);
	CallLocalFunction("IPUI_OnShowAttempt", "iiff", id, playerid, x, y);
	CallLocalFunction("IPUI_OnShown", "ii", id, playerid);
	return 1;
}

stock IPUI_GetAnchor(id, playerid, &Float:x, &Float:y) {
	CallLocalFunction("IPUI_OnAnchorGetAttempt", "iiff", id, playerid, ref(_:x), ref(_:y));
	return 1;
}

stock IPUI_HideForPlayer(id, playerid) {
	if(IPUI_GetPlayerPhone(playerid) == id) {
		map_remove(m_playerPhone, playerid);
		if(!map_size(m_playerPhone)) {
			map_delete(m_playerPhone);
			m_playerPhone = Map:-1;
		}
	}
	CallLocalFunction("IPUI_OnHideAttempt", "ii", id, playerid);
	return 1;
}

stock IPUI_SetScreenColor(id, playerid, color) {
	CallLocalFunction("IPUI_OnScreenColorSet", "iii", id, playerid, color);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerClickTD(playerid, Text:clickedid) {
	if(clickedid == Text:INVALID_TEXT_DRAW) {
		new id = IPUI_GetPlayerPhone(playerid);
		if(id != -1) {
			new bool:active[App] = {false, ...};
			IPApp_GetActiveApps(id, active);
			for(new i = 0; i != sizeof(active); i++) {
				if(active[i]) {
					IPApp_TerminateStats(id, i);
				}
			}
			IPApp_HideUIForPlayer(id, playerid);
			IPSystem_HideUIForPlayer(id, playerid);
			IPUI_HideForPlayer(id, playerid);
		}
	}
	return 1;
}