#if defined call_contact_wrapper_included
	#endinput
#endif

#define call_contact_wrapper_included


forward _prv_Contact_OnNameByNumber(Task:t, const number[]);
public _prv_Contact_OnNameByNumber(Task:t, const number[]) {
	if(!cache_num_rows()) {
		task_set_result_str(t, number);
	} else {
		new name[CONTACT_NAME_SIZE];
		cache_get_value_name(0, "saved_name", name);
		task_set_result_str(t, name);
	}
	return 1;
}

stock Task:Contact_NameByNumberAsync(id, const number[]) {
	new Task:t = task_new();
	if(!IPhone_Has(id) || !IGroup_Has(id)) {
		task_set_result_ms_str(t, number, 0);
		return t;
	}

	new
		sim = -1,
		Iter:it = IGroup_GetIter(id);
	if(!iter_valid(it)) {
		task_set_result_ms_str(t, number, 0);
		return t;
	}
	iter_release(it);
	for(; iter_inside(it); iter_move_next(it)) {
		if(ISim_Has(iter_get(it))) {
			sim = iter_get(it);
			break;
		}
	}

	if(sim == -1) {
		task_set_result_ms_str(t, number, 0);
		return t;
	}

	new query[200];
	mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT a.saved_name FROM appcall_stats a JOIN (SELECT b.id FROM item_sim b WHERE b.number = '%e') b ON b.id = a.saved_sim WHERE a.sim = %d LIMIT 1", number, sim);
	mysql_pquery(MySQL_Connection(), query, "_prv_Contact_OnNameByNumber", "is", _:t, number);
	return t;
}