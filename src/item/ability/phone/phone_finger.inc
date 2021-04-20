#if defined phone_finger_included
	#endinput
#endif

#define phone_finger_included

enum _:FingerStats {
	fingerIndex,
	bool:fingerBodyFocus // 1: body, 0: screen (app)
}

static Map:m_fingerStats;

stock iPhone_FingerLeft(playerid) {
	if(!map_valid(m_fingerStats) || !)
}

stock iPhone_InitFinger(playerid) {
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
