#if defined app_menu_stats_included
	#endinput
#endif

#define app_menu_stats_included

static Map:m_menuStats;

forward AppMenu_OnAppsRetrieved(id, Task:result);
public AppMenu_OnAppsRetrieved(id, Task:result) {
	new LinkedList:ll = linked_list_new();
	if(!map_valid(m_menuStats)) {
		m_menuStats = map_new();
	} else if(map_has_key(m_menuStats, id)) {
		map_remove_deep(m_menuStats, id);
	}
	map_set(m_menuStats, id, _:ll);

	new rows = cache_num_rows();

	for(new app = 0, j = IPApp_GetSize(); app != j; app++) {
		if(app == AppMenu) {
			continue;
		}
		if(!IPApp_IsExternal(app)) {
			linked_list_add(ll, app);
		}
	}
	for(new i = 0, app; i != rows; i++) {
		cache_get_value_name_int(i, "app", app);
		linked_list_add(ll, app);
	}
	if(task_valid(result)) {
		task_set_result(result, rows);
	}
	return 1;
}

stock AppMenu_Has(id) {
	return (map_valid(m_menuStats) && map_has_key(m_menuStats, id));
}

#include <YSI_Coding\y_hooks>

hook IPA_OnStatsInitAttempt(id, app, Task:result) {
	if(app == AppMenu) {
		new query[128];
		mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT * FROM phone_external_apps WHERE id = %d", id);
		mysql_pquery(MySQL_Connection(), query, "AppMenu_OnAppsRetrieved", "ii", id, _:result);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS phone_external_apps (\
		created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
		modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
		id INT UNSIGNED,\
		app TINYINT UNSIGNED,\
		FOREIGN KEY (id) REFERENCES item_phone(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new
		Cache:result = mysql_query(MySQL_Connection(), "SELECT COUNT(*) exists FROM INFORMATION_SCHEMA.STATISTICS WHERE table_schema=DATABASE() AND table_name='phone_external_apps' AND index_name='unique_phone_app'", true),
		count;
	cache_get_value_name_int(0, "exists", count);
	if(!count) {
		mysql_query(MySQL_Connection(), "CREATE UNIQUE INDEX unique_phone_app ON phone_external_apps(id, app)", false);
	}
	cache_delete(result);
	return 1;
}
