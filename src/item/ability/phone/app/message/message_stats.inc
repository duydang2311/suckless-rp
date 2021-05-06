#if defined app_message_stats_included
	#endinput
#endif

#define app_message_stats_included

const MAX_MESSAGES = 10;

enum _:AppMsg_MessageStats {
	msgFromSim,
	msgToSim,
	msgContent[64]
}

enum _:AppMsgStats {
	appMessageOffset,
	appMessages[MAX_MESSAGES]
}

static Map:m_msgStats;

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