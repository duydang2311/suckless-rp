#if defined subtitle_select_included
	#endinput
#endif

#define subtitle_select_included

#define SubtitleSelect_Show(%0,%1,%2,%3) __SubtitleSelect_Show(%0,#%1,%2,%3)
#define SubtitleSelect:%0(%1) \
	forward __SS_%0(%1); \
	public __SS_%0(%1)

#define TOP_LEFT_X 120.0000
#define TOP_LEFT_Y 340.0000
// there are undefinitions at the eof to keep these defines static.

enum _:SubtitleSelectStats {
	PlayerText:subSelectHeaderTD,
	PlayerText:subSelectItemTD,
	PlayerText:subSelectArrowTD,
	subSelectCallback[32],
	subSelectItemText[256],
	subSelectCurrentItem,
	subSelectItemCount
}

static Map:m_subSelectStats;

stock __SubtitleSelect_Show(playerid, const callback[], const header[], const item[]) {
	CallLocalFunction("Subtitle_OnPreShown", "ii", playerid, SubtitleSelect);
	if(!map_valid(m_subSelectStats)) {
		m_subSelectStats = map_new();
	}

	new stats[SubtitleSelectStats];
	if(map_has_key(m_subSelectStats, playerid)) {
		map_get_arr(m_subSelectStats, playerid, stats);
		PlayerTextDrawDestroy(playerid, stats[subSelectHeaderTD]);
		PlayerTextDrawDestroy(playerid, stats[subSelectItemTD]);
		PlayerTextDrawDestroy(playerid, stats[subSelectArrowTD]);
		SetPreciseTimer(stats[subSelectCallback], 0, false, "iii", playerid, 0, -1);
	}

	new pos = 0;
	stats[subSelectCallback][0] = EOS;
	stats[subSelectItemText] = "~g~";
	stats[subSelectCurrentItem] = 0;
	stats[subSelectItemCount] = 1;
	while((pos = strfind(item, "~n~", true, ++pos)) != -1) {
		stats[subSelectItemCount]++;
	}
	strcat(stats[subSelectItemText], item, 256);
	strcat(stats[subSelectCallback], "__SS_", 32);
	strcat(stats[subSelectCallback], callback, 32);
	pos = strfind(item, "~n~", true, 0);
	if(pos != -1) {
		strins(stats[subSelectItemText], "~w~", pos + 3, 256);
	}

	stats[subSelectHeaderTD] = CreatePlayerTextDraw(playerid, TOP_LEFT_X, TOP_LEFT_Y, header);
	PlayerTextDrawFont(playerid, stats[subSelectHeaderTD], 1);
	PlayerTextDrawLetterSize(playerid, stats[subSelectHeaderTD], 0.200000, 0.800000);
	PlayerTextDrawTextSize(playerid, stats[subSelectHeaderTD], 400.000000, 250.000000);
	PlayerTextDrawSetOutline(playerid, stats[subSelectHeaderTD], 1);
	PlayerTextDrawSetShadow(playerid, stats[subSelectHeaderTD], 0);
	PlayerTextDrawAlignment(playerid, stats[subSelectHeaderTD], 1);
	PlayerTextDrawColor(playerid, stats[subSelectHeaderTD], -1);
	PlayerTextDrawBackgroundColor(playerid, stats[subSelectHeaderTD], 255);
	PlayerTextDrawBoxColor(playerid, stats[subSelectHeaderTD], 50);
	PlayerTextDrawUseBox(playerid, stats[subSelectHeaderTD], 0);
	PlayerTextDrawSetProportional(playerid, stats[subSelectHeaderTD], 1);
	PlayerTextDrawSetSelectable(playerid, stats[subSelectHeaderTD], 0);

	stats[subSelectItemTD] = CreatePlayerTextDraw(playerid, TOP_LEFT_X + 10.0, TOP_LEFT_Y + 15.0000, stats[subSelectItemText]);
	PlayerTextDrawFont(playerid, stats[subSelectItemTD], 1);
	PlayerTextDrawLetterSize(playerid, stats[subSelectItemTD], 0.200000, 0.800000);
	PlayerTextDrawTextSize(playerid, stats[subSelectItemTD], 400.000000, 250.000000);
	PlayerTextDrawSetOutline(playerid, stats[subSelectItemTD], 1);
	PlayerTextDrawSetShadow(playerid, stats[subSelectItemTD], 0);
	PlayerTextDrawAlignment(playerid, stats[subSelectItemTD], 1);
	PlayerTextDrawColor(playerid, stats[subSelectItemTD], -1);
	PlayerTextDrawBackgroundColor(playerid, stats[subSelectItemTD], 255);
	PlayerTextDrawBoxColor(playerid, stats[subSelectItemTD], 50);
	PlayerTextDrawUseBox(playerid, stats[subSelectItemTD], 0);
	PlayerTextDrawSetProportional(playerid, stats[subSelectItemTD], 1);
	PlayerTextDrawSetSelectable(playerid, stats[subSelectItemTD], 0);

	stats[subSelectArrowTD] = CreatePlayerTextDraw(playerid, TOP_LEFT_X, TOP_LEFT_Y + 15.0000, "ld_beat:right");
	PlayerTextDrawFont(playerid, stats[subSelectArrowTD], 4);
	PlayerTextDrawLetterSize(playerid, stats[subSelectArrowTD], 0.600000, 2.000000);
	PlayerTextDrawTextSize(playerid, stats[subSelectArrowTD], 9.000000, 9.000000);
	PlayerTextDrawSetOutline(playerid, stats[subSelectArrowTD], 0);
	PlayerTextDrawSetShadow(playerid, stats[subSelectArrowTD], 0);
	PlayerTextDrawAlignment(playerid, stats[subSelectArrowTD], 1);
	PlayerTextDrawColor(playerid, stats[subSelectArrowTD], -1);
	PlayerTextDrawBackgroundColor(playerid, stats[subSelectArrowTD], 9109759);
	PlayerTextDrawBoxColor(playerid, stats[subSelectArrowTD], 50);
	PlayerTextDrawUseBox(playerid, stats[subSelectArrowTD], 1);
	PlayerTextDrawSetProportional(playerid, stats[subSelectArrowTD], 1);
	PlayerTextDrawSetSelectable(playerid, stats[subSelectArrowTD], 0);

	PlayerTextDrawShow(playerid, stats[subSelectHeaderTD]);
	PlayerTextDrawShow(playerid, stats[subSelectItemTD]);
	PlayerTextDrawShow(playerid, stats[subSelectArrowTD]);

	map_set_arr(m_subSelectStats, playerid, stats);
	return 1;
}

stock SubtitleSelect_Hide(playerid) {
	if(!map_valid(m_subSelectStats)) {
		return 0;
	}
	if(!map_has_key(m_subSelectStats, playerid)) {
		return 0;
	}
	new stats[SubtitleSelectStats];
	map_get_arr(m_subSelectStats, playerid, stats);
	PlayerTextDrawDestroy(playerid, stats[subSelectHeaderTD]);
	PlayerTextDrawDestroy(playerid, stats[subSelectItemTD]);
	PlayerTextDrawDestroy(playerid, stats[subSelectArrowTD]);
	map_remove(m_subSelectStats, playerid);
	if(!map_size(m_subSelectStats)) {
		map_delete(m_subSelectStats);
		m_subSelectStats = Map:-1;
	}
	CallLocalFunction("Subtitle_OnHided", "i", playerid);
	return 1;
}

stock SubtitleSelect_UpdateSelect(playerid, value) {
	new
		stats[SubtitleSelectStats];
	map_get_arr(m_subSelectStats, playerid, stats);
	stats[subSelectCurrentItem] += value;
	if(stats[subSelectCurrentItem] >= stats[subSelectItemCount] || stats[subSelectCurrentItem] < 0) {
		return Y_HOOKS_BREAK_RETURN_1;
	}

	PlayerTextDrawDestroy(playerid, stats[subSelectArrowTD]);
	stats[subSelectArrowTD] = CreatePlayerTextDraw(playerid, TOP_LEFT_X, TOP_LEFT_Y + 15.0000 + stats[subSelectCurrentItem] * 7.0000, "ld_beat:right");
	PlayerTextDrawFont(playerid, stats[subSelectArrowTD], 4);
	PlayerTextDrawLetterSize(playerid, stats[subSelectArrowTD], 0.600000, 2.000000);
	PlayerTextDrawTextSize(playerid, stats[subSelectArrowTD], 9.000000, 9.000000);
	PlayerTextDrawSetOutline(playerid, stats[subSelectArrowTD], 0);
	PlayerTextDrawSetShadow(playerid, stats[subSelectArrowTD], 0);
	PlayerTextDrawAlignment(playerid, stats[subSelectArrowTD], 1);
	PlayerTextDrawColor(playerid, stats[subSelectArrowTD], -1);
	PlayerTextDrawBackgroundColor(playerid, stats[subSelectArrowTD], 9109759);
	PlayerTextDrawBoxColor(playerid, stats[subSelectArrowTD], 50);
	PlayerTextDrawUseBox(playerid, stats[subSelectArrowTD], 1);
	PlayerTextDrawSetProportional(playerid, stats[subSelectArrowTD], 1);
	PlayerTextDrawSetSelectable(playerid, stats[subSelectArrowTD], 0);
	PlayerTextDrawShow(playerid, stats[subSelectArrowTD]);
	new pos = strfind(stats[subSelectItemText], "~g~", true);
	if(pos != -1) {
		strdel(stats[subSelectItemText], pos, pos + 3);
	}
	pos = strfind(stats[subSelectItemText], "~w~", true);
	if(pos != -1) {
		strdel(stats[subSelectItemText], pos, pos + 3);
	}

	pos = 0;
	for(new i = 0; i != stats[subSelectCurrentItem]; i++) {
		pos = strfind(stats[subSelectItemText], "~n~", true, pos + 1);
	}
	strins(stats[subSelectItemText], "~g~", (pos == 0) ? pos : pos + 3, 256);
	pos = strfind(stats[subSelectItemText], "~n~", true, pos + 1);
	if(pos != -1) {
		strins(stats[subSelectItemText], "~w~", pos + 3, 256);
	}
	map_set_arr(m_subSelectStats, playerid, stats);
	PlayerTextDrawSetString(playerid, stats[subSelectItemTD], stats[subSelectItemText]);
	PlayerTextDrawShow(playerid, stats[subSelectItemTD]);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(map_valid(m_subSelectStats) && map_has_key(m_subSelectStats, playerid)) {
		if(newkeys & KEY_YES) { // Y -> move up
			SubtitleSelect_UpdateSelect(playerid, -1);
		} else if(newkeys & KEY_NO) { // N -> move down
			SubtitleSelect_UpdateSelect(playerid, 1);
		} else if(newkeys & KEY_SPRINT) { // Space -> select
			new
				stats[SubtitleSelectStats];
			map_get_arr(m_subSelectStats, playerid, stats);
			SetPreciseTimer(stats[subSelectCallback], 0, false, "iii", playerid, 1, stats[subSelectCurrentItem]);
			SubtitleSelect_Hide(playerid);
		} else if(newkeys & KEY_SECONDARY_ATTACK && !(newkeys & KEY_YES)) { // Enter -> cancel
			new
				stats[SubtitleSelectStats];
			map_get_arr(m_subSelectStats, playerid, stats);
			SetPreciseTimer(stats[subSelectCallback], 0, false, "iii", playerid, 0, -1);
			SubtitleSelect_Hide(playerid);
		} else {
			return 1;
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook OnGameModeInit() {
	return 1;
}

#undef TOP_LEFT_X
#undef TOP_LEFT_Y