#if defined app_message_button_ui_included
	#endinput
#endif

#define app_message_button_ui_included

static enum _:Key {
	ItemId,
	PlayerId
}

static Map:m_buttonUI; // { {Key}: PlayerText:button_textdraw }

stock AppMsg_DestroyButtonUI(id, playerid) {
	new key[Key];
	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!map_valid(m_buttonUI) || !map_has_arr_key(m_buttonUI, key)) {
		return 0;
	}

	PlayerTextDrawDestroy(playerid, PlayerText:map_arr_get(m_buttonUI, key));
	map_arr_remove(m_buttonUI, key);
	if(!map_size(m_buttonUI)) {
		map_delete(m_buttonUI);
		m_buttonUI = Map:-1;
	}
	return 1;
}

stock AppMsg_UpdateButtonUI(id, playerid) {
	const Float:BUTTON_SIZE = 10.00;
	new
		key[Key],
		Float:anc[2],
		Float:safe_min[2],
		Float:safe_max[2];
	IPModel_GetSafeScreenOffset(id, safe_min[0], safe_min[1], safe_max[0], safe_max[1]);
	IPUI_GetAnchor(id, playerid, anc[0], anc[1]);

	key[ItemId] = id;
	key[PlayerId] = playerid;
	if(!map_valid(m_buttonUI)) {
		m_buttonUI = map_new();
	} else if(map_has_arr_key(m_buttonUI, key)) {
		AppMsg_DestroyButtonUI(id, playerid);
	}

	new PlayerText:button = CreatePlayerTextDraw(playerid, anc[0] + safe_min[0], anc[1] + safe_max[1] - BUTTON_SIZE / 2.0, "Send");
	PlayerTextDrawFont(playerid, button, 1);
	PlayerTextDrawLetterSize(playerid, button, 0.150000, 0.600000);
	PlayerTextDrawTextSize(playerid, button, anc[0] + safe_min[0] + (safe_max[0] - safe_min[0]) / 2.0, BUTTON_SIZE);
	PlayerTextDrawSetOutline(playerid, button, 1);
	PlayerTextDrawSetShadow(playerid, button, 0);
	PlayerTextDrawAlignment(playerid, button, 1);
	PlayerTextDrawColor(playerid, button, 0x7A7A7AFF);
	PlayerTextDrawBackgroundColor(playerid, button, 255);
	PlayerTextDrawBoxColor(playerid, button, 50);
	PlayerTextDrawUseBox(playerid, button, 1);
	PlayerTextDrawSetProportional(playerid, button, 1);
	PlayerTextDrawSetSelectable(playerid, button, 1);
	PlayerTextDrawShow(playerid, button);
	map_arr_set(m_buttonUI, key, _:button);
	return 1;
}