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

stock IPhone_ShowUIForPlayer(id, playerid, Float:x, Float:y) {
	if(!IPhone_Has(id)) {
		return 0;
	}

	if(!map_valid(m_playerPhone)) {
		m_playerPhone = map_new();
	}
	map_set(m_playerPhone, playerid, id);
	CallLocalFunction("IPUI_OnShowAttempt", "iiff", id, playerid, x, y);
	return 1;
}

stock IPhone_GetUIAnchor(id, playerid, &Float:x, &Float:y) {
	CallLocalFunction("IPUI_OnAnchorGetAttempt", "iiff", id, playerid, ref(_:x), ref(_:y));
	return 1;
}

stock IPhone_HideUIForPlayer(id, playerid) {
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