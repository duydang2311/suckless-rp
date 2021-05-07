#if defined app_message_stats_included
	#endinput
#endif

#define app_message_stats_included

const
	MAX_MESSAGES = 10,
	MESSAGE_CONTENT_SIZE = 64;

enum _:AppMsgStats {
	msgBufferOffset,
	LinkedList:msgList // [ {MessageStats} ]
}

enum _:MessageStats {
	msgId,
	msgFromSim,
	msgContent[MESSAGE_CONTENT_SIZE]
}

static Map:m_msgStats; // { phone_id: {AppMsgStats} }

forward AppMsg_OnStatsRetrieved(id, Task:result);
public AppMsg_OnStatsRetrieved(id, Task:result) {
	new rows = cache_num_rows();
	if(rows) {
		new stats[AppMsgStats];
		stats[msgBufferOffset] = 0;
		stats[msgList] = linked_list_new();
		if(!map_valid(m_msgStats)) {
			m_msgStats = map_new();
		}
		map_set_arr(m_msgStats, id, stats);

		for(new i = 0, msg[MessageStats]; i != rows; i++) {
			cache_get_value_name_int(i, "id", msg[msgId]);
			cache_get_value_name_int(i, "from_sim", msg[msgFromSim]);
			cache_get_value_name(i, "content", msg[msgContent], MESSAGE_CONTENT_SIZE);
			linked_list_add(stats[msgList], msg);
		}
	}
	if(task_valid(result)) {
		task_set_result(result, rows);
	}
	return 1;
}

stock AppMsg_HasStats(id) {
	return (map_valid(m_msgStats) && map_has_key(m_msgStats, id));
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
			for(new sim; iter_inside(it); iter_release(it)) {
				sim = iter_get(it);
				if(ISim_Has(sim)) {
					new query[256];
					mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT id, from_sim, content FROM appmsg_stats WHERE to_sim = %d ORDER BY created_date DESC", sim);
					mysql_pquery(MySQL_Connection(), query, "AppMsg_OnStatsRetrieved", "ii", id, _:result);
				}
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnStatsTermnAttempt(id, app) {
	if(app == AppMessage) {

		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnAppsGetAttempt(id, bool:arr[], size) {
	if(AppMessage < size && AppMsg_HasStats(id)) {
		arr[AppMessage] = true;
	}
	return 1;
}

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(), query, sizeof(query), "CREATE TABLE IF NOT EXISTS appmsg_stats (\
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