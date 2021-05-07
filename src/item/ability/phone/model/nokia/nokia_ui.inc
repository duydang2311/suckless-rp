#if defined nokia_ui_included
	#endinput
#endif

#define nokia_ui_included

static enum _:UIStats {
	PlayerText:UIBody,
	PlayerText:UIScreen,
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
			PlayerTextDrawDestroy(playerid, stats[UIScreen]);
		}
		stats[UIX] = x;
		stats[UIY] = y;

		stats[UIBody] = CreatePlayerTextDraw(playerid, x, y, "phone:body_trans");
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

		stats[UIScreen] = CreatePlayerTextDraw(playerid, x + 13.000000, y + 24.500000, "phone:screen");
		PlayerTextDrawFont(playerid, stats[UIScreen], 4);
		PlayerTextDrawLetterSize(playerid, stats[UIScreen], 0.600000, 2.000000);
		PlayerTextDrawTextSize(playerid, stats[UIScreen], 84.000000, 84.000000);
		PlayerTextDrawSetOutline(playerid, stats[UIScreen], 1);
		PlayerTextDrawSetShadow(playerid, stats[UIScreen], 0);
		PlayerTextDrawAlignment(playerid, stats[UIScreen], 1);
		PlayerTextDrawColor(playerid, stats[UIScreen], -1);
		PlayerTextDrawBackgroundColor(playerid, stats[UIScreen], 255);
		PlayerTextDrawBoxColor(playerid, stats[UIScreen], 50);
		PlayerTextDrawUseBox(playerid, stats[UIScreen], 1);
		PlayerTextDrawSetProportional(playerid, stats[UIScreen], 1);
		PlayerTextDrawSetSelectable(playerid, stats[UIScreen], 0);
		PlayerTextDrawShow(playerid, stats[UIScreen]);

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
			PlayerTextDrawDestroy(playerid, stats[UIScreen]);
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