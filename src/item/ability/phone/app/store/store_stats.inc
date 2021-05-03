#if defined app_store_stats_included
	#endinput
#endif

#define app_store_stats_included

const
	APP_DESCRIPTION_SIZE = 128,
	APP_AUTHOR_SIZE = MAX_PLAYER_NAME + 1,
	APP_VERSION_SIZE = 16;

static enum _:AppStoreStats {
	appIndexOffset
}

enum _:AppStoreRegisteredStats {
	appId,
	appDesc[APP_DESCRIPTION_SIZE],
	appAuthor[APP_AUTHOR_SIZE],
	appVersion[APP_VERSION_SIZE]
}

static
	Map:m_storeStats,
	LinkedList:ll_appStats;

stock AppStore_HasStats(id) {
	return (map_valid(m_storeStats) && map_has_key(m_storeStats, id));
}

stock AppStore_Register(app, const description[] = "", const author[] = "", const version[] = "") {
	new query[400];
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO appstore_stats (app, description, author, version) VALUES (%d, '%e', '%e', '%e') ON DUPLICATE KEY UPDATE description = VALUES(description), author = VALUES(author)", app, description, author, version);
	new Cache:result = mysql_query(MySQL_Connection(), query, true);
	new name[APP_NAME_SIZE];
	IPApp_GetName(app, name);
	cache_delete(result);
	printf("INFO: '%s' has registered to AppStore.", name);
	return 1;
}

stock AppStore_GetIndexOffset(id) {
	if(!AppStore_HasStats(id)) {
		return 0;
	}

	new stats[AppStoreStats];
	map_get_arr(m_storeStats, id, stats);
	return stats[appIndexOffset];
}

stock Iter:AppStore_GetLLAppStatsIter(index = 0) {
	new Iter:it = Iter:-1;
	if(linked_list_valid(ll_appStats)) {
		it = linked_list_iter(ll_appStats, index);
		iter_acquire(it);
	}
	return it;
}

stock AppStore_GetLLAppStatsSize() {
	if(linked_list_valid(ll_appStats)) {
		return linked_list_size(ll_appStats);
	}
	return 0;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnEnsureRequest() {
	IPApp_Ensure(AppStore, "App Store", "app:store", false);
	return 1;
}

hook IPA_OnStatsInitAttempt(id, app, Task:result) {
	if(app == AppStore) {
		// ll_appStats is loaded on gamemode initiation, no asynchornous process will be made.
		if(!map_valid(m_storeStats)) {
			m_storeStats = map_new();
		}
		new stats[AppStoreStats];
		stats[appIndexOffset] = 0;
		map_set_arr(m_storeStats, id, stats);
		if(task_valid(result)) { // if AppStore initiation is called by IPApp_InitStatsAsync, just set the task result anyways to avoid undead task & memory leak
			task_unblock(Y_HOOKS_BREAK_RETURN_1);
			await task_ms(1);
			task_set_result(result, 1);
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnStatsTermnAttempt(id, app) {
	if(app == AppStore) {
		if(AppStore_HasStats(id)) {
			map_remove_deep(m_storeStats, id);
			if(!map_size(m_storeStats)) {
				map_delete(m_storeStats);
				m_storeStats = Map:-1;
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnAppsGetAttempt(id, bool:arr[], size) {
	if(AppStore < size && AppStore_HasStats(id)) {
		arr[AppStore] = true;
	}
	return 1;
}

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS appstore_stats (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	app TINYINT UNSIGNED DEFAULT 0 PRIMARY KEY,\
	description VARCHAR(64) DEFAULT '',\
	version VARCHAR(16) DEFAULT '',\
	author VARCHAR(33) DEFAULT ''\
	)", false);

	CallLocalFunction("AppStore_OnStatsInit", "");
	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM appstore_stats", true);
	new rows = cache_num_rows();
	if(rows) {
		if(!linked_list_valid(ll_appStats)) {
			ll_appStats = linked_list_new();
		}
		for(new i = 0, stats[AppStoreRegisteredStats]; i != rows; i++) {
			cache_get_value_name_int(i, "app", stats[appId]);
			cache_get_value_name(i, "description", stats[appDesc], APP_DESCRIPTION_SIZE);
			cache_get_value_name(i, "author", stats[appAuthor], APP_AUTHOR_SIZE);
			cache_get_value_name(i, "version", stats[appVersion], APP_VERSION_SIZE);
			linked_list_add_arr(ll_appStats, stats);
		}
	}
	cache_delete(result);
	printf("INFO: AppStore - Loaded %d registered apps.", rows);
	return 1;
}