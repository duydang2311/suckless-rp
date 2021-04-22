#if defined app_stats_included
	#endinput
#endif

#define app_stats_included

const APP_NAME_SIZE = 32;

enum _:App {
	AppMenu,
	AppStore,
	AppNow
}

enum _:AppStats {
	appName[APP_NAME_SIZE],
	bool:appExternal
}

static appStats[App][AppStats];

stock IPApp_Has(app) {
	return (app >= 0 && app < sizeof(appStats));
}

stock IPApp_GetSize() {
	return sizeof(appStats);
}

stock IPApp_IsExternal(app) {
	if(!IPApp_Has(app)) {
		return 0;
	}

	return appStats[app][appExternal];
}

stock IPApp_GetName(app, name[], size = sizeof(name)) {
	if(!IPApp_Has(app)) {
		return 0;
	}

	name[0] = EOS;
	strcat(name, appStats[app][appName]);
	return 1;
}

stock Task:IPApp_InitStats(id, app) {
	if(!IPhone_Has(id) || !IPApp_Has(app)) {
		return 0;
	}

	new
		Task:t = task_new(),
		Task:t_result = task_new();
	task_set_result_ms(t_result, 0, 2000);
	CallLocalFunction("IPApp_OnStatsInitAttempt", "iii", id, app, t_result);
	task_yield(t);
	task_set_result(t, task_await(t_result));
	return Task:0;
}

stock IPApp_ShowUIForPlayer(id, app, playerid) {
	if(!IPhone_Has(id) || !IPApp_Has(app) || !AppMenu_Has(id)) {
		return 0;
	}

	CallLocalFunction("IPApp_OnUIShowAttempt", "iii", id, app, playerid);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS app_stats (\
		created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
		modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
		app TINYINT UNSIGNED PRIMARY KEY,\
		name VARCHAR(32) DEFAULT '',\
		external BOOLEAN DEFAULT TRUE\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM app_stats", true);
	new rows = cache_num_rows();
	for(new i = 0, app; i != rows; i++) {
		cache_get_value_name_int(i, "app", app);
		cache_get_value_name(i, "name", appStats[app][appName], APP_NAME_SIZE);
		cache_get_value_name_int(i, "external", appStats[app][appExternal]);
	}
	cache_delete(result);
	printf("INFO: Loaded %d phone application stats.", rows);
	return 1;
}