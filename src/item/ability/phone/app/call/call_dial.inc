#if defined app_call_dial_included
	#endinput
#endif

#define app_call_dial_included

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

				if(!ISim_GetNumber(from_sim, from_number)) {
					task_unblock(1);
					await_arr(from_number) ISim_RetrieveNumber(from_sim);
				}

				SetPreciseTimer("AppCall_OnDialed", "ii", sim, to_sim);
			}
		}
	}
	return 0;
}