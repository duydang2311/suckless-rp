#if defined app_menu_stats_included
	#endinput
#endif

#define app_menu_stats_included

static enum _:AppMenuStats {
	appIndexOffset,
	LinkedList:appList
}

static Map:m_menuStats; // { item_id: {AppMenuStats} }

forward AppMenu_OnAppAdded(id, app, Task:t);
public AppMenu_OnAppAdded(id, app, Task:t) {
	new rows = cache_affected_rows();
	if(rows) {
		if(AppMenu_HasStats(id)) {
			new stats[AppMenuStats];
			map_get_arr(m_menuStats, id, stats);
			if(!linked_list_valid(stats[appList])) {
				stats[appList] = linked_list_new();
			}
			linked_list_add(stats[appList], app);
		}
	}
	task_set_result(t, rows);
	return 1;
}

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
			printf("add app %d to appList", app);
		}
	}

	new rows = cache_num_rows();
	for(new i = 0, app; i != rows; i++) {
		cache_get_value_name_int(i, "app", app);
		linked_list_add(stats[appList], app);
		printf("add app %d to appList", app);
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

stock AppMenu_GetAppListSize(id) {
	if(!AppMenu_HasStats(id)) {
		return 0;
	}
	new stats[AppMenuStats];
	map_get_arr(m_menuStats, id, stats);
	if(linked_list_valid(stats[appList])) {
		return linked_list_size(stats[appList]);
	}
	return 0;
}

stock AppMenu_IsAppInstalled(id, app) {
	if(AppMenu_HasStats(id)) {
		new stats[AppMenuStats];
		map_get_arr(m_menuStats, id, stats);
		if(linked_list_valid(stats[appList])) {
			for(new Iter:it = linked_list_iter(stats[appList]); iter_inside(it); iter_move_next(it)) {
				if(iter_get(it) == app) {
					return 1;
				}
			}
		}
	}
	return 0;
}

stock Task:AppMenu_AddAppAsync(id, app) {
	new
		query[128],
		Task:t = task_new();
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO appmenu_apps (id, app) VALUES (%d, %d) ON DUPLICATE KEY UPDATE id = id", id, app);
	mysql_pquery(MySQL_Connection(), query, "AppMenu_OnAppAdded", "iii", id, app, _:t);
	return t;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnEnsureRequest() {
	IPApp_Ensure(AppMenu, "App Menu", "app:menu", false);
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
		mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT * FROM appmenu_apps WHERE id = %d ORDER BY id ASC", id);
		mysql_pquery(MySQL_Connection(), query, "AppMenu_OnAppsRetrieved", "ii", id, _:result);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnStatsTermnAttempt(id, app) {
	if(app == AppMenu) {
		if(AppMenu_HasStats(id)) {
			new stats[AppMenuStats];
			map_get_arr(m_menuStats, id, stats);
			linked_list_delete(stats[appList]);
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

hook AppMenu_RequestInstall(id, app, &Task:t) {
	t = AppMenu_AddAppAsync(id, app);
	return 1;
}

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS appmenu_apps (\
		created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
		modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
		id INT UNSIGNED,\
		app TINYINT UNSIGNED,\
		FOREIGN KEY (id) REFERENCES item_phone(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new
		Cache:result = mysql_query(MySQL_Connection(), "SELECT COUNT(*) FROM INFORMATION_SCHEMA.STATISTICS WHERE table_schema=DATABASE() AND table_name='appmenu_apps' AND index_name='unique_phone_app'", true);
	new count = 0;
	cache_get_value_name_int(0, "COUNT(*)", count);
	if(!count) {
		mysql_query(MySQL_Connection(), "CREATE UNIQUE INDEX unique_phone_app ON appmenu_apps(id, app)", false);
	}
	cache_delete(result);
	return 1;
}