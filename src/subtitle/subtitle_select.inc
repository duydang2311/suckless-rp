#if defined subtitle_select_included
	#endinput
#endif

#define subtitle_select_included

enum _:SubtitleSelectStats {
	PlayerText:subSelectHeaderTD,
	PlayerText:subSelectItemTD,
	PlayerText:subSelectArrowTD,
	subSelectCallback[32]
}

static Map:m_subSelectStats;

stock SubtitleSelect_Show(playerid, const header[], const item[], const callback[]) {
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
	}

	stats[subSelectHeaderTD] = CreatePlayerTextDraw(playerid, 100.000000, 340.000000, header);
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

	stats[subSelectItemTD] = CreatePlayerTextDraw(playerid, 110.000000, 355.000000, item);
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

	stats[subSelectArrowTD] = CreatePlayerTextDraw(playerid, 100.000000, 355.000000, "ld_beat:right");
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
	stats[subSelectCallback][0] = EOS;
	strcat(stats[subSelectCallback], callback, 32);
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

#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(map_valid(m_subSelectStats) && map_has_key(m_subSelectStats, playerid)) {
		if(newkeys & KEY_YES) { // Y -> move up

		} else if(newkeys & KEY_NO) { // N -> move down

		} else if(newkeys & KEY_SPRINT) { // Space -> select

		} else if(newkeys & KEY_SECONDARY_ATTACK) { // Enter -> cancel
			SubtitleSelect_Hide(playerid);
		} else {
			return 1;
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}