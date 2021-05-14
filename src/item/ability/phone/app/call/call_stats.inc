#if defined app_call_stats_included
	#endinput
#endif

#define app_call_stats_included

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

		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnStatsTermnAttempt(id, app) {
	if(app == AppCall) {

		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnAppsGetAttempt(id, bool:arr[], size) {
	if(AppCall < size && (AppCall_HasDetailStats(id) || AppCall_HasListStats(id))) {
		arr[AppCall] = true;
	}
	return 1;
}

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS appcall_stats (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	sim INT UNSIGNED PRIMARY KEY,\
	saved_sim INT UNSIGNED,\
	saved_name VARCHAR(33) DEFAULT '',\
	FOREIGN KEY (sim) REFERENCES item_sim(id) ON UPDATE CASCADE ON DELETE CASCADE,\
	FOREIGN KEY (saved_sim) REFERENCES item_sim(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new
		Cache:result = mysql_query(MySQL_Connection(), "SELECT COUNT(*) FROM INFORMATION_SCHEMA.STATISTICS WHERE table_schema=DATABASE() AND table_name='appmenu_call' AND index_name='unique_saved_sim'", true);
	new count = 0;
	cache_get_value_name_int(0, "COUNT(*)", count);
	if(!count) {
		mysql_query(MySQL_Connection(), "CREATE UNIQUE INDEX unique_saved_sim ON appmenu_apps(sim, saved_sim)", false);
	}
	cache_delete(result);
	return 1;
}