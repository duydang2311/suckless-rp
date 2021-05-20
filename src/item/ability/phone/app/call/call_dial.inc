#if defined app_call_dial_included
	#endinput
#endif

#define app_call_dial_included

const APP_CALL_DIAL_DELAY_MS = 1000;

enum _:CallDialStats {
	dialSimNumber[SIM_NUMBER_SIZE],
	bool:dialUsingSpeaker
}

static Map:m_dialStats; // { phone_id: {CallDialStats} }

stock AppCall_HasDialStats(id) {
	return (map_valid(m_dialStats) && map_has_key(m_dialStats, id));
}

stock AppCall_GetDialStats(id, stats[CallDialStats]) {
	if(!AppCall_HasDialStats(id)) {
		return 0;
	}

	map_get_arr(m_dialStats, id, stats);
	return 1;
}

stock AppCall_GetDialSimNumber(id, number[SIM_NUMBER_SIZE]) {
	if(!AppCall_HasDialStats(id)) {
		return 0;
	}

	new stats[CallDialStats];
	map_get_arr(m_dialStats, id, stats);
	number[0] = EOS;
	strcat(number, stats[dialSimNumber]);
	return 1;
}

stock bool:AppCall_IsDialUsingSpeaker(id) {
	if(!AppCall_HasDialStats(id)) {
		return false;
	}

	new stats[CallDialStats];
	map_get_arr(m_dialStats, id, stats);
	return stats[dialUsingSpeaker];
}

stock AppCall_Dial(id, const to_number[]) {
	if(!IPhone_Has(id) || !IGroup_Has(id)) {
		return 0;
	}

	new Iter:it = IGroup_GetIter(id);
	if(iter_valid(it) && iter_release(it)) {
		for(new sim; iter_inside(it); iter_move_next(it)) {
			sim = iter_get(it);
			if(ISim_Has(sim)) {
				new stats[CallDialStats];
				strcat(stats[dialSimNumber], to_number, SIM_NUMBER_SIZE);
				if(isnull(stats[dialSimNumber])) {
					strcat(stats[dialSimNumber], "Unknown");
				}
				stats[dialUsingSpeaker] = false;
				if(!map_valid(m_dialStats)) {
					m_dialStats = map_new();
				}
				map_set_arr(m_dialStats, id, stats);
				CallLocalFunction("AppCall_OnPreDialed", "is", sim, stats[dialSimNumber]);
				SetPreciseTimer("AppCall_OnDialed", APP_CALL_DIAL_DELAY_MS, false, "is", sim, stats[dialSimNumber]);
			}
		}
	}
	return 0;
}