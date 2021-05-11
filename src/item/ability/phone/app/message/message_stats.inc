#if defined app_message_stats_included
	#endinput
#endif

#define app_message_stats_included

const
	MAX_MESSAGES = 10,
	MESSAGE_CONTENT_SIZE = 64;

enum _:AppMsgStats {
	msgOffset,
	LinkedList:msgList // [ {MessageStats} ]
}

enum _:MessageStats {
	msgId,
	msgFromSim,
	msgContent[MESSAGE_CONTENT_SIZE]
}

static
	Map:m_listStats, // { phone_id: {AppMsgStats} }
	Map:m_detailStats; // { phone_id: {AppMsgStats} }

forward AppMsg_OnListStatsRetrieved(id, Task:result);
public AppMsg_OnListStatsRetrieved(id, Task:result) {
	new rows = cache_num_rows();
	if(rows) {
		new stats[AppMsgStats];
		stats[msgOffset] = 0;
		stats[msgList] = linked_list_new();
		if(!map_valid(m_listStats)) {
			m_listStats = map_new();
		}
		map_set_arr(m_listStats, id, stats);

		for(new i = 0, msg[MessageStats]; i != rows; i++) {
			cache_get_value_name_int(i, "id", msg[msgId]);
			cache_get_value_name_int(i, "from_sim", msg[msgFromSim]);
			cache_get_value_name(i, "content", msg[msgContent], MESSAGE_CONTENT_SIZE);
			linked_list_add_arr(stats[msgList], msg);
		}
	}
	if(task_valid(result)) {
		task_set_result(result, rows);
	}
	return 1;
}

stock AppMsg_HasListStats(id) {
	return (map_valid(m_listStats) && map_has_key(m_listStats, id));
}

stock AppMsg_HasDetailStats(id) {
	return (map_valid(m_detailStats) && map_has_key(m_detailStats, id));
}

stock AppMsg_GetListOffset(id) {
	if(!AppMsg_HasListStats(id)) {
		return 0;
	}
	new stats[AppMsgStats];
	map_get_arr(m_listStats, id, stats);
	return stats[msgOffset];
}

stock AppMsg_SetListOffset(id, offset) {
	if(!AppMsg_HasListStats(id)) {
		return 0;
	}
	map_set_cell(m_listStats, id, msgOffset, offset);
	return 1;
}

stock AppMsg_SetDetailsOffset(id, offset) {
	if(!AppMsg_HasDetailsStats(id)) {
		return 0;
	}
	map_set_cell(m_detailsStats, id, msgOffset, offset);
	return 1;
}

stock AppMsg_GetDetailOffset(id) {
	if(!AppMsg_HasDetailStats(id)) {
		return 0;
	}
	new stats[AppMsgStats];
	map_get_arr(m_detailStats, id, stats);
	return stats[msgOffset];
}

stock Iter:AppMsg_GetListIter(id, offset = 0) {
	new Iter:it = Iter:-1;
	if(AppMsg_HasListStats(id)) {
		new stats[AppMsgStats];
		map_get_arr(m_listStats, id, stats);
		if(linked_list_valid(stats[msgList])) {
			it = linked_list_iter(stats[msgList], offset);
			iter_acquire(it);
		}
	}
	return it;
}

stock Iter:AppMsg_GetDetailIter(id, offset) {
	new Iter:it = Iter:-1;
	if(AppMsg_HasDetailStats(id)) {
		new stats[AppMsgStats];
		map_get_arr(m_detailStats, id, stats);
		if(linked_list_valid(stats[msgList])) {
			it = linked_list_iter(stats[msgList], offset);
			iter_acquire(it);
		}
	}
	return it;
}

stock AppMsg_GetListSize(id) {
	if(!AppMsg_HasListStats(id)) {
		return 0;
	}

	new stats[AppMsgStats];
	map_get_arr(m_listStats, id, stats);
	if(!linked_list_valid(stats[msgList])) {
		return 0;
	}

	return linked_list_size(stats[msgList]);
}

forward AppMsg_OnDetailsStatsRetrieved(id, from_sim, Task:t);
public AppMsg_OnDetailsStatsRetrieved(id, from_sim, Task:t) {
	new rows = cache_num_rows();
	if(rows) {
		new stats[AppMsgStats];
		if(!AppMsg_HasDetailStats(id)) {
			stats[msgOffset] = 0;
			stats[msgList] = linked_list_new();
			if(!map_valid(m_detailStats)) {
				m_detailStats = map_new();
			}
			map_set_arr(m_detailStats, id, stats);
		} else {
			map_get_arr(m_detailStats, id, stats);
		}

		for(new i = 0, msg[MessageStats]; i != rows; i++) {
			cache_get_value_name_int(i, "id", msg[msgId]);
			cache_get_value_name(i, "content", msg[msgContent], MESSAGE_CONTENT_SIZE);
			cache_get_value_name_int(i, "from_sim", msg[msgFromSim]);

			linked_list_add_arr(stats[msgList], msg);
		}
	}
	task_set_result(t, rows);
	return 1;
}

stock Task:AppMsg_RetrieveDetailsStats(id, from_sim) {
	// this function retrieves the next 10 rows based on (rowOffset / 10) in enum AppMsgStats of Map:m_detailStats
	new
		Task:t = task_new(),
		query[220];
	mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT id, content, from_sim FROM appmsg_stats WHERE (from_sim = %d AND to_sim = %d) OR  (from_sim = %d AND to_sim = %d) ORDER BY created_date DESC LIMIT %d, %d", from_sim, to_sim, to_sim, from_sim, floatround(float(stats[msgOffset]) / 10.0, floatround_floor) * 10, (floatround(float(stats[msgOffset]) / 10.0, floatround_floor) + 1) * 10 - 1);
	mysql_pquery(MySQL_Connection(), query, "AppMsg_OnDetailsStatsRetrieved", "iii", id, from_sim, _:t);
	return t;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnEnsureRequest() {
	IPApp_Ensure(AppMessage, "Message", "app:message", false);
	return 1;
}

hook IPA_OnStatsInitAttempt(id, app, Task:result) {
	if(app == AppMessage) {
		if(!IGroup_Has(id)) { // this phone has no sim
			return Y_HOOKS_BREAK_RETURN_1;
		}

		new Iter:it = IGroup_GetIter(id);
		if(iter_valid(it) && iter_release(it)) {
			for(new sim; iter_inside(it); iter_move_next(it)) {
				sim = iter_get(it);
				if(ISim_Has(sim)) {
					new query[512];
					mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT stats.id, stats.from_sim, stats.content FROM appmsg_stats stats INNER JOIN (\
					SELECT to_sim, MAX(created_date) LatestDate FROM appmsg_stats GROUP BY from_sim) grouped_stats ON stats.to_sim = grouped_stats.to_sim WHERE stats.to_sim = %d AND stats.created_date = grouped_stats.LatestDate ORDER BY created_date DESC", sim);
					mysql_pquery(MySQL_Connection(), query, "AppMsg_OnListStatsRetrieved", "ii", id, _:result);
				}
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnStatsTermnAttempt(id, app) {
	if(app == AppMessage) {
		if(AppMsg_HasDetailStats(id)) {
			map_remove_deep(m_detailStats, id);
			if(!map_size(m_detailStats)) {
				map_delete(m_detailStats);
				m_detailStats = Map:-1;
			}
		}
		if(AppMsg_HasListStats(id)) {
			map_remove_deep(m_listStats, id);
			if(!map_size(m_listStats)) {
				map_delete(m_listStats);
				m_listStats = Map:-1;
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnAppsGetAttempt(id, bool:arr[], size) {
	if(AppMessage < size && (AppMsg_HasDetailStats(id) || AppMsg_HasListStats(id))) {
		arr[AppMessage] = true;
	}
	return 1;
}

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS appmsg_stats (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
	from_sim INT UNSIGNED,\
	to_sim INT UNSIGNED,\
	content VARCHAR(64) DEFAULT '',\
	FOREIGN KEY (to_sim) REFERENCES item_sim(id) ON UPDATE CASCADE ON DELETE CASCADE,\
	FOREIGN KEY (from_sim) REFERENCES item_sim(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}