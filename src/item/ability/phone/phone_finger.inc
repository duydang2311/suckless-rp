#if defined phone_finger_included
	#endinput
#endif

#define phone_finger_included

enum _:FingerStats {
	fingerIndex,
	PlayerText:fingerTd,
	bool:fingerBodyFocus, // 1: body, 0: screen (app)
}

static enum _:FingerStatsKey {
	PlayerId,
	ItemId
}

static Map:m_fingerStats;

stock IPFinger_Has(const key[], size = sizeof(key)) {
	return (map_valid(m_fingerStats) && map_has_arr_key(m_fingerStats, key, size));
}

stock IPFinger_MoveLeft(playerid, id) {
	new key[FingerStatsKey];
	key[PlayerId] = playerid;
	key[ItemId] = id;
	if(!IPFinger_Has(key)) {
		return -1;
	}

	new stats[FingerStats];
	map_arr_get_arr(m_fingerStats, key, stats);
	if(stats[fingerBodyFocus]) {
		stats[fingerIndex]--;
		CallLocalFunction("IPM_ValidateIndex", "ii", id, ref(stats[fingerIndex]));
		if(stats[fingerIndex] != -1) {
			map_arr_set_cell(m_fingerStats, key, fingerIndex, stats[fingerIndex]);
		}
		IPFinger_UpdateTextDraw(playerid, id);
	} else {

	}
	return stats[fingerIndex];
}

stock IPFinger_MoveRight(playerid, id) {
	new key[FingerStatsKey];
	key[PlayerId] = playerid;
	key[ItemId] = id;
	if(!IPFinger_Has(key)) {
		return -1;
	}

	new stats[FingerStats];
	map_arr_get_arr(m_fingerStats, key, stats);
	if(stats[fingerBodyFocus]) {
		stats[fingerIndex]++;
		CallLocalFunction("IPM_ValidateIndex", "ii", id, ref(stats[fingerIndex]));
		if(stats[fingerIndex] != -1) {
			map_arr_set_cell(m_fingerStats, key, fingerIndex, stats[fingerIndex]);
		}
		IPFinger_UpdateTextDraw(playerid, id);
	} else {

	}
	return stats[fingerIndex];
}

stock IPFinger_MoveUp(playerid, id) {
	new key[FingerStatsKey];
	key[PlayerId] = playerid;
	key[ItemId] = id;
	if(!IPFinger_Has(key)) {
		return -1;
	}

	new stats[FingerStats];
	map_arr_get_arr(m_fingerStats, key, stats);
	if(stats[fingerBodyFocus]) {
		CallLocalFunction("IPM_CalculateUp", "ii", id, ref(stats[fingerIndex]));
		if(stats[fingerIndex] != -1) {
			map_arr_set_cell(m_fingerStats, key, fingerIndex, stats[fingerIndex]);
		}
		IPFinger_UpdateTextDraw(playerid, id);
	} else {

	}
	return stats[fingerIndex];
}

stock IPFinger_MoveDown(playerid, id) {
	new key[FingerStatsKey];
	key[PlayerId] = playerid;
	key[ItemId] = id;
	if(!IPFinger_Has(key)) {
		return -1;
	}

	new stats[FingerStats];
	map_arr_get_arr(m_fingerStats, key, stats);
	if(stats[fingerBodyFocus]) {
		CallLocalFunction("IPM_CalculateDown", "ii", id, ref(stats[fingerIndex]));
		if(stats[fingerIndex] != -1) {
			map_arr_set_cell(m_fingerStats, key, fingerIndex, stats[fingerIndex]);
		}
		IPFinger_UpdateTextDraw(playerid, id);
	} else {

	}
	return stats[fingerIndex];
}

stock IPFinger_GetCurrentIndex(playerid, id) {
	new key[FingerStatsKey];
	key[PlayerId] = playerid;
	key[ItemId] = id;
	if(!IPFinger_Has(key)) {
		return -1;
	}
	new stats[FingerStats];
	map_arr_get_arr(m_fingerStats, key, stats);
	return stats[fingerIndex];
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

stock IPFinger_Init(playerid, id, index = 0) {
	if(!IPhone_Has(id)) {
		return 0;
	}

	new
		stats[FingerStats],
		key[FingerStatsKey];
	key[PlayerId] = playerid;
	key[ItemId] = id;
	if(!map_valid(m_fingerStats)) {
		m_fingerStats = map_new();
	} else if(map_has_arr_key(m_fingerStats, key)) {
		map_arr_get_arr(m_fingerStats, key, stats);
		PlayerTextDrawDestroy(playerid, stats[fingerTd]);
	}

	new
		Float:x,
		Float:y,
		Float:anc_x,
		Float:anc_y,
		Float:size;
	IPhone_GetUIAnchor(id, playerid, anc_x, anc_y);
	IPModel_GetBodyOffset(id, index, x, y, size);
	stats[fingerIndex] = 0;
	stats[fingerBodyFocus] = true;
	stats[fingerTd] = IPFinger_CreateTextDraw(playerid, anc_x + x, anc_y + y, size);
	PlayerTextDrawShow(playerid, stats[fingerTd]);
	map_arr_set_arr(m_fingerStats, key, stats);
	return 1;
}

stock IPFinger_UpdateTextDraw(playerid, id) {
	new key[FingerStatsKey];
	key[PlayerId] = playerid;
	key[ItemId] = id;
	if(!IPFinger_Has(key)) {
		return -1;
	}

	new
		stats[FingerStats],
		Float:anc_x,
		Float:anc_y,
		Float:x,
		Float:y,
		Float:size;
	map_arr_get_arr(m_fingerStats, key, stats);
	IPhone_GetUIAnchor(id, playerid, anc_x, anc_y);
	IPModel_GetBodyOffset(id, stats[fingerIndex], x, y, size);
	PlayerTextDrawDestroy(playerid, stats[fingerTd]);
	stats[fingerTd] = IPFinger_CreateTextDraw(playerid, anc_x + x, anc_y + y, size);
	PlayerTextDrawShow(playerid, stats[fingerTd]);
	map_arr_set_cell(m_fingerStats, key, fingerTd, _:stats[fingerTd]);
	return 1;
}

stock IPFinger_Hide(playerid, id) {
	new key[FingerStatsKey];
	key[PlayerId] = playerid;
	key[ItemId] = id;
	if(!IPFinger_Has(key)) {
		return -1;
	}

	new stats[FingerStats];
	map_arr_get_arr(m_fingerStats, key, stats);
	PlayerTextDrawDestroy(playerid, stats[fingerTd]);
	map_arr_remove(m_fingerStats, key);
	if(!map_size(m_fingerStats)) {
		map_delete(m_fingerStats);
		m_fingerStats = Map:-1;
	}
	return 1;
}

#include <YSI_Coding\y_hooks>
