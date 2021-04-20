#if defined phone_finger_included
	#endinput
#endif

#define phone_finger_included

enum _:FingerStats {
	fingerIndex,
	PlayerText:fingerTd,
	bool:fingerBodyFocus, // 1: body, 0: screen (app)
}

static Map:m_fingerStats;

stock IPFinger_MoveLeft(playerid, model) {
	if(!map_valid(m_fingerStats) || !map_has_key(m_fingerStats, playerid)) {
		return -1;
	}

	new stats[FingerStats];
	map_get_arr(m_fingerStats, playerid, stats);
	if(stats[fingerBodyFocus]) {
		stats[fingerIndex]--;
		CallLocalFunction("IPhoneBody_ValidateIndex", "ii", model, ref(stats[fingerIndex]));
		if(stats[fingerIndex] != -1) {
			map_set_cell(m_fingerStats, playerid, fingerIndex, stats[fingerIndex]);
		}
		IPFinger_UpdateTextDraw(playerid, model);
	} else {

	}
	return stats[fingerIndex];
}

stock IPFinger_MoveRight(playerid, model) {
	if(!map_valid(m_fingerStats) || !map_has_key(m_fingerStats, playerid)) {
		return -1;
	}

	new stats[FingerStats];
	map_get_arr(m_fingerStats, playerid, stats);
	if(stats[fingerBodyFocus]) {
		stats[fingerIndex]++;
		CallLocalFunction("IPhoneBody_ValidateIndex", "ii", model, ref(stats[fingerIndex]));
		if(stats[fingerIndex] != -1) {
			map_set_cell(m_fingerStats, playerid, fingerIndex, stats[fingerIndex]);
		}
		IPFinger_UpdateTextDraw(playerid, model);
	} else {

	}
	return stats[fingerIndex];
}

stock IPFinger_MoveUp(playerid, model) {
	if(!map_valid(m_fingerStats) || !map_has_key(m_fingerStats, playerid)) {
		return -1;
	}

	new stats[FingerStats];
	map_get_arr(m_fingerStats, playerid, stats);
	if(stats[fingerBodyFocus]) {
		CallLocalFunction("IPhoneBody_CalculateUp", "ii", model, ref(stats[fingerIndex]));
		if(stats[fingerIndex] != -1) {
			map_set_cell(m_fingerStats, playerid, fingerIndex, stats[fingerIndex]);
		}
		IPFinger_UpdateTextDraw(playerid, model);
	} else {

	}
	return stats[fingerIndex];
}

stock IPFinger_MoveDown(playerid, model) {
	if(!map_valid(m_fingerStats) || !map_has_key(m_fingerStats, playerid)) {
		return -1;
	}

	new stats[FingerStats];
	map_get_arr(m_fingerStats, playerid, stats);
	if(stats[fingerBodyFocus]) {
		CallLocalFunction("IPhoneBody_CalculateDown", "ii", model, ref(stats[fingerIndex]));
		if(stats[fingerIndex] != -1) {
			map_set_cell(m_fingerStats, playerid, fingerIndex, stats[fingerIndex]);
		}
		IPFinger_UpdateTextDraw(playerid, model);
	} else {

	}
	return stats[fingerIndex];
}

stock IPFinger_GetDisplayStats(model, index, &Float:x, &Float:y, &Float:size) {
	new result;
	CallLocalFunction("IPhoneBody_GetButtonStats", "iifffi", model, index, ref(_:x), ref(_:y), ref(_:size), ref(result));
	return result;
}

stock PlayerText:IPFinger_CreateTextDraw(playerid, Float:x, Float:y, Float:size) {
	new PlayerText:td = CreatePlayerTextDraw(playerid, x, y, "ld_pool:ball");
	PlayerTextDrawFont(playerid, td, 4);
	PlayerTextDrawLetterSize(playerid, td, 0.00000, 0.000000);
	PlayerTextDrawTextSize(playerid, td, size, size);
	PlayerTextDrawSetOutline(playerid, td, 0);
	PlayerTextDrawSetShadow(playerid, td, 0);
	PlayerTextDrawAlignment(playerid, td, 2);
	PlayerTextDrawColor(playerid, td, -65436);
	PlayerTextDrawBackgroundColor(playerid, td, 255);
	PlayerTextDrawBoxColor(playerid, td, 255);
	PlayerTextDrawUseBox(playerid, td, 1);
	PlayerTextDrawSetProportional(playerid, td, 1);
	PlayerTextDrawSetSelectable(playerid, td, 0);
	return td;
}

stock IPFinger_Init(playerid, model, index = 0) {
	new stats[FingerStats];
	if(!map_valid(m_fingerStats)) {
		m_fingerStats = map_new();
	} else if(map_has_key(m_fingerStats, playerid)) {
		map_get_arr(m_fingerStats, playerid, stats);
		PlayerTextDrawDestroy(playerid, stats[fingerTd]);
	}

	new
		Float:x,
		Float:y,
		Float:anc_x,
		Float:anc_y,
		Float:size;
	IPhone_GetUIAnchor(model, playerid, anc_x, anc_y);
	IPFinger_GetDisplayStats(model, index, x, y, size);
	stats[fingerIndex] = 0;
	stats[fingerBodyFocus] = true;
	stats[fingerTd] = IPFinger_CreateTextDraw(playerid, anc_x + x, anc_y + y, size);
	PlayerTextDrawShow(playerid, stats[fingerTd]);
	map_set_arr(m_fingerStats, playerid, stats);
	return 1;
}

stock IPFinger_UpdateTextDraw(playerid, model) {
	if(!map_valid(m_fingerStats) || !map_has_key(m_fingerStats, playerid)) {
		return 0;
	}

	new
		stats[FingerStats],
		Float:anc_x,
		Float:anc_y,
		Float:x,
		Float:y,
		Float:size;
	map_get_arr(m_fingerStats, playerid, stats);
	IPhone_GetUIAnchor(model, playerid, anc_x, anc_y);
	IPFinger_GetDisplayStats(model, stats[fingerIndex], x, y, size);
	PlayerTextDrawDestroy(playerid, stats[fingerTd]);
	stats[fingerTd] = IPFinger_CreateTextDraw(playerid, anc_x + x, anc_y + y, size);
	PlayerTextDrawShow(playerid, stats[fingerTd]);
	map_set_cell(m_fingerStats, playerid, fingerTd, _:stats[fingerTd]);
	return 1;
}

#include <YSI_Coding\y_hooks>
