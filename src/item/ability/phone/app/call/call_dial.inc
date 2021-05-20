#if defined app_call_dial_included
	#endinput
#endif

#define app_call_dial_included

const APP_CALL_DIAL_DELAY_MS = 1000;

enum _:CallDialStats {
	dialSim,
	bool:dialUsingSpeaker
}

static Map:m_dialStats; // { phone_id: {CallDialStats} }

stock AppCall_HasDialStats(id) {
	return (map_valid(m_dialStats) && map_has_key(m_dialStats, id));
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
				stats[dialSim] = ISim_GetSimWithNumber(to_number);
				stats[dialUsingSpeaker] = false;
				if(!map_valid(m_dialStats)) {
					m_dialStats = map_new();
				}
				map_set_arr(m_dialStats, id, stats);
				CallLocalFunction("AppCall_OnPreDialed", "ii", sim, stats[dialSim]);
				SetPreciseTimer("AppCall_OnDialed", APP_CALL_DIAL_DELAY_MS, false, "ii", sim, stats[dialSim]);
			}
		}
	}
	return 0;
}