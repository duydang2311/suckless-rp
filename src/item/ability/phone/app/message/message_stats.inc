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
	Map:m_detailStats, // { phone_id: {AppMsgStats} }
	Map:m_simFromDetail; // { phone_id: sim_id }

forward AppMsg_OnListStatsRetrieved(id, for_sim, Task:result);
public AppMsg_OnListStatsRetrieved(id, for_sim, Task:result) {
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
			if(msg[msgFromSim] == for_sim) {
				cache_get_value_name_int(i, "to_sim", msg[msgFromSim]);
			}
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

stock AppMsg_SetDetailOffset(id, offset) {
	if(!AppMsg_HasDetailStats(id)) {
		return 0;
	}
	map_set_cell(m_detailStats, id, msgOffset, offset);
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

stock AppMsg_GetSimFromDetail(id) {
	if(!map_valid(m_simFromDetail) || !map_has_key(m_simFromDetail, id)) {
		return -1;
	}
	return map_get(m_simFromDetail, id);
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

stock AppMsg_GetDetailSize(id) {
	if(!AppMsg_HasDetailStats(id)) {
		return 0;
	}

	new stats[AppMsgStats];
	map_get_arr(m_detailStats, id, stats);
	if(!linked_list_valid(stats[msgList])) {
		return 0;
	}

	return linked_list_size(stats[msgList]);
}

forward AppMsg_OnDetailStatsRetrieved(id, from_sim, Task:t);
public AppMsg_OnDetailStatsRetrieved(id, from_sim, Task:t) {
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
		if(!map_valid(m_simFromDetail)) {
			m_simFromDetail = map_new();
		}
		map_set(m_simFromDetail, id, from_sim);
	}
	task_set_result(t, rows);
	return 1;
}

stock Task:AppMsg_RetrieveDetailStats(id, from_sim, advance = 0) {
	if(!IGroup_Has(id)) {
		return Task:0;
	}
	new Iter:it = IGroup_GetIter(id);
	if(iter_valid(it) && iter_release(it)) {
		for(new sim; iter_inside(it); iter_move_next(it)) {
			sim = iter_get(it);
			if(ISim_Has(sim)) {
				new
					Task:t = task_new(),
					query[220],
					offset = 0 + advance;
				if(AppMsg_HasDetailStats(id)) {
					new stats[AppMsgStats];
					map_get_arr(m_detailStats, id, stats);
					offset += stats[msgOffset];
				}
				mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT id, content, from_sim FROM appmsg_stats WHERE (from_sim = %d AND to_sim = %d) OR  (from_sim = %d AND to_sim = %d) ORDER BY created_date DESC LIMIT %d, %d", from_sim, sim, sim, from_sim, floatround(float(offset) / 10.0, floatround_floor) * 10, (floatround(float(offset) / 10.0, floatround_floor) + 1) * 10 - 1);
				mysql_pquery(MySQL_Connection(), query, "AppMsg_OnDetailStatsRetrieved", "iii", id, from_sim, _:t);
				return t;
			}
		}
	}
	return Task:0;
}

stock AppMsg_DeleteMsgByIter(Iter:it) {
	if(!iter_linked(it)) {
		return 0;
	}

	new
		msg[MessageStats],
		query[128];
	iter_get_arr(it, msg);
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM appmsg_stats WHERE id = %d LIMIT 1", msg[msgId]);
	mysql_pquery(MySQL_Connection(), query);

	iter_erase(it);
	return 1;
}

stock Task:AppMsg_SendAsync(from_sim, to_sim, const content[]) {
	new
		Task:t = task_new(),
		safe_content[MESSAGE_CONTENT_SIZE],
		query[256];
	strcat(safe_content, content); // just to make sure
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO appmsg_stats (from_sim, to_sim, content) VALUES (%d, %d, '%e')", from_sim, to_sim, content);
	mysql_pquery(MySQL_Connection(), query, "AppMsg_OnSentAsync", "i", _:t);
	return t;
}

stock AppMsg_ListPushFront(id, msg_id, from_sim, const content[]) {
	new stats[AppMsgStats];
	if(!map_valid(m_listStats)) {
		m_listStats = map_new();
	}
	if(!map_has_key(m_listStats, id)) {
		stats[msgOffset] = 0;
		stats[msgList] = linked_list_new();
		map_set_arr(m_listStats, id, stats);
	} else if(map_has_key(m_listStats, id)) {
		map_get_arr(m_listStats, id, stats);
	}

	new msg[MessageStats];
	msg[msgId] = msg_id;
	strcat(msg[msgContent], content, SIM_NUMBER_SIZE);
	msg[msgFromSim] = from_sim;
	linked_list_add_arr(stats[msgList], msg, 0);
	return 1;
}

stock AppMsg_DetailPushFront(id, msg_id, from_sim, const content[]) {
	new stats[AppMsgStats];
	if(!map_valid(m_detailStats)) {
		m_detailStats = map_new();
		stats[msgOffset] = 0;
		stats[msgList] = linked_list_new();
		map_set_arr(m_detailStats, id, stats);
	} else if(map_has_key(m_detailStats, id)) {
		map_get_arr(m_detailStats, id, stats);
	}

	new msg[MessageStats];
	msg[msgId] = msg_id;
	strcat(msg[msgContent], content);
	msg[msgFromSim] = from_sim;
	linked_list_add_arr(stats[msgList], msg, 0);
	return 1;
}

forward AppMsg_OnSentAsync(Task:t);
public AppMsg_OnSentAsync(Task:t) {
	new id = -1;
	if(cache_affected_rows()) {
		id = cache_insert_id();
	}
	task_set_result(t, id);
	return 1;
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
					mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT m.id, m.from_sim, m.to_sim, m.content from appmsg_stats m \
						inner join ( \
							select \
								least(from_sim, to_sim) as user_1, \
								greatest(from_sim, to_sim) as user_2, \
								max(id) as last_id, \
								max(created_date) as latest_date \
								from \
								appmsg_stats \
							WHERE to_sim = %d OR from_sim = %d \
							group by \
							least(from_sim, to_sim), \
							greatest(from_sim, to_sim) \
						) s on least(from_sim, to_sim)=user_1 \
							and greatest(from_sim, to_sim)=user_2 \
							and m.id = s.last_id \
						ORDER BY m.created_date DESC", sim, sim);

					mysql_pquery(MySQL_Connection(), query, "AppMsg_OnListStatsRetrieved", "iii", id, sim, _:result);
					break;
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
		if(map_valid(m_simFromDetail) && map_has_key(m_simFromDetail, id)) {
			map_remove(m_simFromDetail, id);
			if(!map_size(m_simFromDetail)) {
				map_delete(m_simFromDetail);
				m_simFromDetail = Map:-1;
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

hook AppMsg_StatsOnMsgSent(msg_id, from_sim, to_sim, const content[]) {
	printf("AppMsg_StatsOnMsgSent(%d, %d, %d, %s)", msg_id, from_sim, to_sim, content);
	new safe_content[MESSAGE_CONTENT_SIZE];
	strcat(safe_content, content);
	if(map_valid(m_detailStats)) {
		for(new Iter:it = map_iter(m_detailStats), id, Iter:phone_it, cnt = 0; iter_inside(it) && cnt != 2; iter_move_next(it)) {
			id = iter_get_key(it);
			if(!IGroup_Has(id)) {
				continue;
			}

			phone_it = IGroup_GetIter(id);
			if(iter_valid(phone_it) && iter_release(phone_it)) {
				for(new sim; iter_inside(phone_it); iter_move_next(phone_it)) {
					sim = iter_get(phone_it);
					if(sim == from_sim || sim == to_sim) {
						AppMsg_DetailPushFront(id, msg_id, from_sim, safe_content);
						cnt++;
					}
				}
			}
		}
	}

	if(map_valid(m_listStats)) {
		for(new Iter:it = map_iter(m_listStats), id, Iter:phone_it, cnt = 0; iter_inside(it) && cnt != 2; iter_move_next(it)) {
			id = iter_get_key(it);
			if(!IGroup_Has(id)) {
				continue;
			}

			phone_it = IGroup_GetIter(id);
			if(iter_valid(phone_it) && iter_release(phone_it)) {
				for(new sim; iter_inside(phone_it); iter_move_next(phone_it)) {
					sim = iter_get(phone_it);
					if(sim == from_sim || sim == to_sim) {
						new Iter:list_it = AppMsg_GetListIter(id);
						if(iter_valid(list_it) && iter_release(list_it)) {
							for(new stats[MessageStats]; iter_inside(list_it); iter_move_next(list_it)) {
								iter_get_arr(list_it, stats);
								if(stats[msgFromSim] == to_sim || stats[msgFromSim] == from_sim) {
									iter_erase(list_it);
									break;
								}
							}
						}
						AppMsg_ListPushFront(id, msg_id, to_sim, safe_content);
						cnt++;
						break;
					}
				}
			}
		}
	}
	return 1;
}