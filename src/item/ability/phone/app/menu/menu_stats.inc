#if defined app_menu_stats_included
	#endinput
#endif

#define app_menu_stats_included

static enum _:AppMenuStats {
	appIndexOffset,
	LinkedList:appList
}

static Map:m_menuStats; // { item_id: {AppMenuStats} }

forward AppMenu_OnAppsRetrieved(id, Task:result);
public AppMenu_OnAppsRetrieved(id, Task:result) {
	new stats[AppMenuStats];
	if(!map_valid(m_menuStats)) {
		m_menuStats = map_new();
		stats[appIndexOffset] = 0;
	} else if(map_has_key(m_menuStats, id)) {
		map_get_arr(m_menuStats, id, stats);
		if(linked_list_valid(stats[appList])) {
			linked_list_delete(stats[appList]);
		}
	}
	stats[appList] = linked_list_new();
	map_set_arr(m_menuStats, id, stats);

	for(new app = 0, j = IPApp_GetSize(); app != j; app++) {
		if(app == AppMenu) {
			continue;
		}
		if(!IPApp_IsExternal(app)) {
			linked_list_add(stats[appList], app);
		}
	}

	new rows = cache_num_rows();
	for(new i = 0, app; i != rows; i++) {
		cache_get_value_name_int(i, "app", app);
		linked_list_add(stats[appList], app);
	}
	if(task_valid(result)) {
		task_set_result(result, rows);
	}
	return 1;
}

stock AppMenu_HasStats(id) {
	return (map_valid(m_menuStats) && map_has_key(m_menuStats, id));
}

stock AppMenu_GetIndexOffset(id) {
	if(!AppMenu_HasStats(id)) {
		return 0;
	}

	new stats[AppMenuStats];
	map_get_arr(m_menuStats, id, stats);
	return stats[appIndexOffset];
}

stock Iter:AppMenu_GetAppListIter(id, index = 0) {
	new Iter:it = Iter:-1;
	if(!AppMenu_HasStats(id)) {
		return it;
	}
	new stats[AppMenuStats];
	map_get_arr(m_menuStats, id, stats);
	if(linked_list_valid(stats[appList])) {
		it = linked_list_iter(stats[appList], index);
		iter_acquire(it);
	}
	return it;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnEnsureRequest() {
	IPApp_Ensure(AppMenu, "App Menu", "app_sprites:setting", false);
	return 1;
}

hook IPA_OnStatsInitAttempt(id, app, Task:result) {
	if(app == AppMenu) {
		new
			query[128],
			stats[AppMenuStats];
		stats[appIndexOffset] = 0;
		stats[appList] = LinkedList:-1;
		if(!map_valid(m_menuStats)) {
			m_menuStats = map_new();
		}
		map_set_arr(m_menuStats, id, stats);
		mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT * FROM phone_external_apps WHERE id = %d ORDER BY id ASC", id);
		mysql_pquery(MySQL_Connection(), query, "AppMenu_OnAppsRetrieved", "ii", id, _:result);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnStatsTermnAttempt(id, app) {
	if(app == AppMenu) {
		if(AppMenu_HasStats(id)) {
			map_remove_deep(m_menuStats, id);
			if(!map_size(m_menuStats)) {
				map_delete(m_menuStats);
				m_menuStats = Map:-1;
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnAppsGetAttempt(id, bool:arr[], size) {
	if(AppMenu < size && AppMenu_HasStats(id)) {
		arr[AppMenu] = true;
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
		Cache:result = mysql_query(MySQL_Connection(), "SELECT COUNT(*) FROM INFORMATION_SCHEMA.STATISTICS WHERE table_schema=DATABASE() AND table_name='phone_external_apps' AND index_name='unique_phone_app'", true);
	new count = 0;
	cache_get_value_name_int(0, "COUNT(*)", count);
	if(!count) {
		mysql_query(MySQL_Connection(), "CREATE UNIQUE INDEX unique_phone_app ON phone_external_apps(id, app)", false);
	}
	cache_delete(result);
	return 1;
}
