#if defined app_call_dial_included
	#endinput
#endif

#define app_call_dial_included

enum _:CallDialStats {
	dialSim,
	dial
}

const APP_CALL_DIAL_DELAY_MS = 1000;

stock AppCall_Dial(id, const to_number[]) {
	if(!IPhone_Has(id) || !IGroup_Has(id)) {
		return 0;
	}

	new Iter:it = IGroup_GetIter(id);
	if(iter_valid(it) && iter_release(it)) {
		for(new sim; iter_inside(it); iter_move_next(it)) {
			sim = iter_get(it);
			if(ISim_Has(sim)) {
				new to_sim;
				to_sim = ISim_GetSimWithNumber(to_number);
				CallLocalFunction("AppCall_OnPreDialed", "ii", sim, to_sim);
				SetPreciseTimer("AppCall_OnDialed", APP_CALL_DIAL_DELAY_MS, false, "ii", sim, to_sim);
			}
		}
	}
	return 0;
}