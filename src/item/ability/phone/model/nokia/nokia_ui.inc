#if defined nokia_ui_included
	#endinput
#endif

#define nokia_ui_included

static enum _:UIStats {
	PlayerText:UIBody,
	Float:UIX,
	Float:UIY,
	UISize
}

static Map:m_uiStats;

#include <YSI_Coding\y_hooks>

hook IPUI_OnAnchorGetAttempt(model, playerid, &Float:x, &Float:y) {
	if(model == Nokia) {
		if(map_valid(m_uiStats) && map_has_key(m_uiStats, playerid)) {
			new stats[UIStats];
			map_get_arr(m_uiStats, playerid, stats);
			x = stats[UIX];
			y = stats[UIY];
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPUI_OnShowAttempt(model, playerid, Float:x, Float:y) {
	if(model == Nokia) {
		new stats[UIStats];
		if(!map_valid(m_uiStats)) {
			m_uiStats = map_new();
		} else if(map_has_key(m_uiStats, playerid)) {
			map_get_arr(m_uiStats, playerid, stats);
			PlayerTextDrawDestroy(playerid, stats[UIBody]);
		}
		stats[UIX] = x;
		stats[UIY] = y;
		stats[UIBody] = CreatePlayerTextDraw(playerid, x, y, "phone:body");
		PlayerTextDrawFont(playerid, stats[UIBody], 4);
		PlayerTextDrawLetterSize(playerid, stats[UIBody], 0.600000, 2.000000);
		PlayerTextDrawTextSize(playerid, stats[UIBody], 200.000000, 200.000000);
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
		map_set_arr(m_uiStats, playerid, stats);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}