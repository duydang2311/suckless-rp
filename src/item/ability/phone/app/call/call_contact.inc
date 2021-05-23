#if defined app_call_contact_included
	#endinput
#endif

#define app_call_contact_included

const CONTACT_NAME_SIZE = 33;

enum _:ContactStats {
	ContactSim,
	ContactName[CONTACT_NAME_SIZE]
}

static enum _:ContactListPage {
	pageOffset,
	LinkedList:pageContactList // LinkedList<{ContactStats}>
}

static Map:m_pageStats; // { phone_id: {ContactListPage} }

forward AppCall_OnSavedSimRetrieved(id, Task:result);
public AppCall_OnSavedSimRetrieved(id, Task:result) {
	new
		rows = cache_num_rows(),
		stats[ContactListPage];
	if(!map_valid(m_pageStats)) {
		m_pageStats = map_new();
	}
	if(!map_has_key(m_pageStats, id)) {
		stats[pageContactList] = linked_list_new();
		stats[pageOffset] = 0;
		map_set_arr(m_pageStats, id, stats);
	} else {
		map_get_arr(m_pageStats, id, stats);
	}
	if(rows) {
		for(new i = 0, contact[ContactStats]; i != rows; i++) {
			cache_get_value_name_int(i, "saved_sim", contact[ContactSim]);
			cache_get_value_name(i, "saved_name", contact[ContactName], CONTACT_NAME_SIZE);
			linked_list_add_arr(stats[pageContactList], contact);
		}
	}
	if(task_valid(result)) {
		task_set_result(result, rows);
	}
	return 1;
}

forward _appcall_onInserted(Task:t);
public _appcall_onInserted(Task:t) {
	if(task_valid(t)) {
		task_set_result(t, cache_affected_rows());
	}
	return 1;
}

forward _appcall_onDeleted(Task:t);
public _appcall_onDeleted(Task:t) {
	if(task_valid(t)) {
		task_set_result(t, cache_affected_rows());
	}
	return 1;
}

forward AppCall_OnModDateUpdated(Task:t);
public AppCall_OnModDateUpdated(Task:t) {
	task_set_result(t, cache_affected_rows());
	return 1;
}

stock AppCall_HasStats(id) {
	return (map_valid(m_pageStats) && map_has_key(m_pageStats, id));
}

stock Task:AppCall_RetrieveStats(id, advance = 0) {
	new Iter:it = IGroup_GetIter(id);
	if(iter_valid(it) && iter_release(it)) {
		for(new sim; iter_inside(it); iter_move_next(it)) {
			sim = iter_get(it);
			if(ISim_Has(sim)) {
				new query[128],
					Task:t = task_new(),
					offset = advance;
				if(AppCall_HasStats(id)) {
					new stats[ContactListPage];
					map_get_arr(m_pageStats, id, stats);
					offset += stats[pageOffset];
				}
				mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT stats.saved_sim, stats.saved_name FROM appcall_stats stats WHERE sim = %d ORDER BY modified_date DESC LIMIT %d, %d", sim, floatround(float(offset) / 10.0, floatround_floor) * 10, (floatround(float(offset) / 10.0, floatround_floor) + 1) * 10 - 1);
				mysql_pquery(MySQL_Connection(), query, "AppCall_OnSavedSimRetrieved", "ii", id, _:t);
				return t;
			}
		}
	}
	return Task:0;
}

stock AppCall_GetOffset(id) {
	if(!AppCall_HasStats(id)) {
		return 0;
	}
	new stats[ContactListPage];
	map_get_arr(m_pageStats, id, stats);
	return stats[pageOffset];
}

stock Iter:AppCall_GetIter(id, offset = 0) {
	new Iter:it = Iter:-1;
	if(AppCall_HasStats(id)) {
		new stats[ContactListPage];
		map_get_arr(m_pageStats, id, stats);
		if(linked_list_valid(stats[pageContactList])) {
			it = linked_list_iter(stats[pageContactList], offset);
			iter_acquire(it);
		}
	}
	return it;
}

stock AppCall_GetListSize(id) {
	if(AppCall_HasStats(id)) {
		new stats[ContactListPage];
		map_get_arr(m_pageStats, id, stats);
		if(linked_list_valid(stats[pageContactList])) {
			return linked_list_size(stats[pageContactList]);
		}
	}
	return 0;
}

stock Task:AppCall_AddContact(sim, saved_sim, const saved_name[]) {
	new
		safe_saved_name[CONTACT_NAME_SIZE],
		Task:t = task_new();
	strcat(safe_saved_name, saved_name);
	new query[172];
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO appcall_stats (sim, saved_sim, saved_name) VALUES (%d, %d, '%e')", sim, saved_sim, saved_name);
	mysql_pquery(MySQL_Connection(), query, "_appcall_onInserted", "i", _:t);
	return t;
}

stock Task:AppCall_DeleteContact(sim, saved_sim) {
	new
		Task:t = task_new(),
		query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM appcall_stats WHERE sim = %d AND saved_sim = %d LIMIT 1", sim, saved_sim);
	mysql_pquery(MySQL_Connection(), query, "_appcall_onDeleted", "i", _:t);
	return t;
}

stock Task:AppCall_UpdateModDate(sim, saved_sim) {
	new
		query[128],
		Task:t = task_new();
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE appcall_stats SET modifed_date = CURRENT_TIMESTAMP WHERE sim = %d, saved_sim = %d LIMIT 1", sim, saved_sim);
	mysql_pquery(MySQL_Connection(), query, "AppCall_OnModDateUpdated", "i", _:t);
	return 1;
}

stock AppCall_GetListOffset(id) {
	if(AppCall_HasStats(id)) {
		new stats[ContactListPage];
		map_get_arr(m_pageStats, id, stats);
		return stats[pageOffset];
	}
	return 0;
}

stock AppCall_SetListOffset(id, offset) {
	if(AppCall_HasStats(id)) {
		map_set_cell(m_pageStats, id, pageOffset, offset);
		return 1;
	}
	return 0;
}

stock Task:AppCall_RetrieveContact(sim, saved_sim) {
	new
		query[128],
		Task:t = task_new();
	mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT stats.saved_sim, stats.saved_name FROM appcall_stats stats WHERE sim = %d AND saved_sim = %d LIMIT 1", sim, saved_sim);
	mysql_pquery(MySQL_Connection(), query, "AppCall_OnContRetrieved", "i", _:t);
	return t;
}

forward AppCall_OnContRetrieved(Task:t);
public AppCall_OnContRetrieved(Task:t) {
	new contact[ContactStats] = {-1, ""};
	if(cache_num_rows()) {
		cache_get_value_name_int(0, "saved_sim", contact[ContactSim]);
		cache_get_value_name(0, "saved_name", contact[ContactName], CONTACT_NAME_SIZE);
	}
	if(task_valid(t)) {
		task_set_result_arr(t, contact);
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnEnsureRequest() {
	IPApp_Ensure(AppCall, "Call", "app:call", false);
	return 1;
}

hook IPA_OnStatsInitAttempt(id, app, Task:result) {
	if(app == AppCall) {
		if(!IGroup_Has(id)) {
			return Y_HOOKS_BREAK_RETURN_1;
		}

		task_unblock(Y_HOOKS_BREAK_RETURN_1);
		new rows = await AppCall_RetrieveStats(id);
		if(task_valid(result)) {
			task_set_result(result, rows);
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnStatsTermnAttempt(id, app) {
	if(app == AppCall) {
		if(AppCall_HasStats(id)) {
			map_remove_deep(m_pageStats, id);
			if(!map_size(m_pageStats)) {
				map_delete(m_pageStats);
				m_pageStats = Map:-1;
			}
		}
		CallLocalFunction("AppCall_OnTermnAttempt", "i", id);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnAppsGetAttempt(id, bool:arr[], size) {
	if(AppCall < size && AppCall_HasStats(id)) {
		arr[AppCall] = true;
	}
	return 1;
}

HOOK__ AppCall_OnContAdded(id, saved_sim, const saved_name[]) {
	printf("HOOK__ AppCall_OnContAdded(%d, %d, %s)", id, saved_sim, saved_name);
	if(AppCall_HasStats(id)) {
		new stats[ContactListPage];
		map_get_arr(m_pageStats, id, stats);
		if(!linked_list_valid(stats[pageContactList])) {
			stats[pageContactList] = linked_list_new();
			map_set_cell(m_pageStats, id, pageContactList, stats[pageContactList]);
		}
		new contact[ContactStats];
		contact[ContactSim] = saved_sim;
		strcat(contact[ContactName], saved_name, CONTACT_NAME_SIZE);
		linked_list_add_arr(stats[pageContactList], contact, 0);
	}
	return 1;
}

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS appcall_stats (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	sim INT UNSIGNED,\
	saved_sim INT UNSIGNED,\
	saved_name VARCHAR(33) DEFAULT '',\
	FOREIGN KEY (sim) REFERENCES item_sim(id) ON UPDATE CASCADE ON DELETE CASCADE,\
	FOREIGN KEY (saved_sim) REFERENCES item_sim(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new
		Cache:result = mysql_query(MySQL_Connection(), "SELECT COUNT(*) FROM INFORMATION_SCHEMA.STATISTICS WHERE table_schema=DATABASE() AND table_name='appcall_stats' AND index_name='unique_saved_sim'", true);
	new count = 0;
	cache_get_value_name_int(0, "COUNT(*)", count);
	if(!count) {
		mysql_query(MySQL_Connection(), "CREATE UNIQUE INDEX unique_saved_sim ON appcall_stats(sim, saved_sim)", false);
	}
	cache_delete(result);
	return 1;
}