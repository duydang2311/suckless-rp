#if defined subtitle_text_included
	#endinput
#endif

#define subtitle_text_included

static
	Map:m_subTextTdId,
	Map:m_subExpireTimer;

forward SubtitleText_Hide(playerid);
public SubtitleText_Hide(playerid) {
	if(!map_valid(m_subTextTdId)) {
		return 0;
	}
	if(!map_has_key(m_subTextTdId, playerid)) {
		return 0;
	}
	if(map_valid(m_subExpireTimer) && map_has_key(m_subExpireTimer, playerid)) {
		map_remove(m_subExpireTimer, playerid);
		if(!map_size(m_subExpireTimer)) {
			map_delete(m_subExpireTimer);
			m_subExpireTimer = Map:-1;
		}
	}
	PlayerTextDrawDestroy(playerid, PlayerText:map_get(m_subTextTdId, playerid));
	map_remove(m_subTextTdId, playerid);
	if(!map_size(m_subTextTdId)) {
		map_delete(m_subTextTdId);
		m_subTextTdId = Map:-1;
	}
	return 1;
}

stock SubtitleText_Show(playerid, const text[], duration = 5000) {
	CallLocalFunction("Subtitle_OnPreShown", "ii", playerid, SubtitleText);
	if(!map_valid(m_subTextTdId)) {
		m_subTextTdId = map_new();
	} else if(map_has_key(m_subTextTdId, playerid)) {
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
	if(duration >= 0) {
		if(!map_valid(m_subExpireTimer)) {
			m_subExpireTimer = map_new();
		} else if(map_has_key(m_subExpireTimer, playerid)) {
			DeletePreciseTimer(map_get(m_subExpireTimer, playerid));
		}
		map_set(m_subExpireTimer, playerid, SetPreciseTimer("SubtitleText_Hide", duration, false, "i", playerid));
	}
	return 1;
}