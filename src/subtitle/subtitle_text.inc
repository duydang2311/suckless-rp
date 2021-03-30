#if defined subtitle_text_included
	#endinput
#endif

#define subtitle_text_included

static Map:m_subTextTdId;

stock SubtitleText_Show(playerid, const text[]) {
	CallLocalFunction("Subtitle_OnPreShown", "ii", playerid, SubtitleText);
	if(!map_valid(m_subTextTdId)) {
		m_subTextTdId = map_new();
	}
	if(map_has_key(m_subTextTdId, playerid)) {
		PlayerTextDrawDestroy(playerid, PlayerText:map_get(m_subTextTdId, playerid));
	}
	new PlayerText:td = CreatePlayerTextDraw(playerid, 320.000000, 340.000000, text);
	PlayerTextDrawFont(playerid, td, 1);
	PlayerTextDrawLetterSize(playerid, td, 0.200000, 0.800000);
	PlayerTextDrawTextSize(playerid, td, 400.000000, 250.000000);
	PlayerTextDrawSetOutline(playerid, td, 1);
	PlayerTextDrawSetShadow(playerid, td, 0);
	PlayerTextDrawAlignment(playerid, td, 2);
	PlayerTextDrawColor(playerid, td, -1);
	PlayerTextDrawBackgroundColor(playerid, td, 255);
	PlayerTextDrawBoxColor(playerid, td, 50);
	PlayerTextDrawUseBox(playerid, td, 0);
	PlayerTextDrawSetProportional(playerid, td, 1);
	PlayerTextDrawSetSelectable(playerid, td, 0);
	PlayerTextDrawShow(playerid, td);
	map_set(m_subTextTdId, playerid, _:td);
	return 1;
}

stock SubtitleText_Hide(playerid) {
	if(!map_valid(m_subTextTdId)) {
		return 0;
	}
	if(!map_has_key(m_subTextTdId, playerid)) {
		return 0;
	}
	PlayerTextDrawDestroy(playerid, PlayerText:map_get(m_subTextTdId, playerid));
	map_remove(m_subTextTdId, playerid);
	if(!map_size(m_subTextTdId)) {
		map_delete(m_subTextTdId);
		m_subTextTdId = Map:-1;
	}
	return 1;
}