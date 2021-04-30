#if defined nokia_ui_included
	#endinput
#endif

#define nokia_ui_included

static enum _:UIStats {
	PlayerText:UIBody,
	Float:UIX,
	Float:UIY
}

static enum _:UIStatsKey {
	ItemId,
	PlayerId
}

static Map:m_uiStats;

stock NokiaUI_Has(const key[], size = sizeof(key)) {
	return (map_valid(m_uiStats) && map_has_arr_key(m_uiStats, key, size));
}

#include <YSI_Coding\y_hooks>

hook IPUI_OnAnchorGetAttempt(id, playerid, &Float:x, &Float:y) {
	if(IPhone_GetModel(id) == Nokia) {
		new key[UIStatsKey];
		key[ItemId] = id;
		key[PlayerId] = playerid;
		if(NokiaUI_Has(key)) {
			new stats[UIStats];
			map_arr_get_arr(m_uiStats, key, stats);
			x = stats[UIX];
			y = stats[UIY];
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPUI_OnShowAttempt(id, playerid, Float:x, Float:y) {
	if(IPhone_GetModel(id) == Nokia) {
		new
			stats[UIStats],
			key[UIStatsKey];
		key[ItemId] = id;
		key[PlayerId] = playerid;
		if(!map_valid(m_uiStats)) {
			m_uiStats = map_new();
		} else if(map_has_arr_key(m_uiStats, key)) {
			map_arr_get_arr(m_uiStats, key, stats);
			PlayerTextDrawDestroy(playerid, stats[UIBody]);
		}
		stats[UIX] = x;
		stats[UIY] = y;
		stats[UIBody] = CreatePlayerTextDraw(playerid, x, y, "phone:body");
		PlayerTextDrawFont(playerid, stats[UIBody], 4);
		PlayerTextDrawLetterSize(playerid, stats[UIBody], 0.600000, 2.000000);
		PlayerTextDrawTextSize(playerid, stats[UIBody], 110.000000, 220.000000);
		PlayerTextDrawSetOutline(playerid, stats[UIBody], 1);
		PlayerTextDrawSetShadow(playerid, stats[UIBody], 0);
		PlayerTextDrawAlignment(playerid, stats[UIBody], 1);
		PlayerTextDrawColor(playerid, stats[UIBody], -1);
		PlayerTextDrawBackgroundColor(playerid, stats[UIBody], 255);
		PlayerTextDrawBoxColor(playerid, stats[UIBody], 50);
		PlayerTextDrawUseBox(playerid, stats[UIBody], 1);
		PlayerTextDrawSetProportional(playerid, stats[UIBody], 1);
		PlayerTextDrawSetSelectable(playerid, stats[UIBody], 0);
		PlayerTextDrawShow(playerid, stats[UIBody]);
		map_arr_set_arr(m_uiStats, key, stats);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPUI_OnHideAttempt(id, playerid) {
	if(IPhone_GetModel(id) == Nokia) {
		new key[UIStatsKey];
		key[ItemId] = id;
		key[PlayerId] = playerid;
		if(NokiaUI_Has(key)) {
			new stats[UIStats];
			map_arr_get_arr(m_uiStats, key, stats);
			PlayerTextDrawDestroy(playerid, stats[UIBody]);
			map_arr_remove(m_uiStats, key);
			if(!map_size(m_uiStats)) {
				map_delete(m_uiStats);
				m_uiStats = Map:-1;
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}