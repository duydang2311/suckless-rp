#if defined phone_finger_included
	#endinput
#endif

#define phone_finger_included

enum _:FingerStats {
	fingerIndex,
	bool:fingerBodyFocus // 1: body, 0: screen (app)
}

static Map:m_fingerStats;

stock IPhone_FingerMoveLeft(playerid, model) {
	if(!map_valid(m_fingerStats) || !map_has_key(m_fingerStats, playerid)) {
		return -1;
	}

	new stats[FingerStats];
	map_get_arr(m_fingerStats, playerid, stats);
	if(fingerBodyFocus) {
		stats[fingerIndex]--;
		CallLocalFunction("IPhoneBody_ValidateIndex", "ii", model, ref(stats[fingerIndex]));
		if(stats[fingerIndex] != -1) {
			map_set_cell(m_fingerStats, playerid, fingerIndex, stats[fingerIndex]);
		}
	} else {

	}
	return stats[fingerIndex];
}

stock IPhone_FingerMoveRight(playerid, model) {
	if(!map_valid(m_fingerStats) || !map_has_key(m_fingerStats, playerid)) {
		return -1;
	}

	new stats[FingerStats];
	map_get_arr(m_fingerStats, playerid, stats);
	if(fingerBodyFocus) {
		stats[fingerIndex]++;
		CallLocalFunction("IPhoneBody_ValidateIndex", "ii", model, ref(stats[fingerIndex]));
		if(stats[fingerIndex] != -1) {
			map_set_cell(m_fingerStats, playerid, fingerIndex, stats[fingerIndex]);
		}
	} else {

	}
	return stats[fingerIndex];
}

stock IPhone_FingerMoveUp(playerid, model) {
	if(!map_valid(m_fingerStats) || !map_has_key(m_fingerStats, playerid)) {
		return -1;
	}

	new stats[FingerStats];
	map_get_arr(m_fingerStats, playerid, stats);
	if(fingerBodyFocus) {
		CallLocalFunction("IPhoneBody_CalculateUp", "ii", model, ref(stats[fingerIndex]));
		if(stats[fingerIndex] != -1) {
			map_set_cell(m_fingerStats, playerid, fingerIndex, stats[fingerIndex]);
		}
	} else {

	}
	return stats[fingerIndex];
}

stock IPhone_FingerMoveDown(playerid, model) {
	if(!map_valid(m_fingerStats) || !map_has_key(m_fingerStats, playerid)) {
		return -1;
	}

	new stats[FingerStats];
	map_get_arr(m_fingerStats, playerid, stats);
	if(fingerBodyFocus) {
		CallLocalFunction("IPhoneBody_CalculateDown", "ii", model, ref(stats[fingerIndex]));
		if(stats[fingerIndex] != -1) {
			map_set_cell(m_fingerStats, playerid, fingerIndex, stats[fingerIndex]);
		}
	} else {

	}
	return stats[fingerIndex];
}

stock IPhone_FingerGetDisplayStats(model, index, &Float:x, &Float:y, &Float:radius) {
	new result;
	CallLocalFunction("IPhoneBody_GetButtonStats", "iifffi", model, index, ref(_:x), ref(_:y), ref(_:radius), ref(result));
	return result;
}

stock IPhone_InitFinger(playerid) {
	if(!map_valid(m_fingerStats)) {
		m_fingerStats = map_new();
	}

	new stats[FingerStats];
	stats[fingerIndex] = 0;
	stats[fingerBodyFocus] = 1;
	map_set_arr(m_fingerStats, playerid, stats)
	return 1;
}

#include <YSI_Coding\y_hooks>
