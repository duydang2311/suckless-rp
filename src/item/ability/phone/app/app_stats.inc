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

stock IPApp_IsExternal(app) {
	return external[app];
}

stock Task:IPApp_Init(id, app) {
	if(!IPhone_Has(id)) {
		return 0;
	}

	new
		Task:t = task_new(),
		Task:t_result = task_new();
	task_set_result_ms(t_result, 0, 2000);
	CallLocalFunction("IPApp_OnInitAttempt", "iii", id, app, t_result);
	task_yield(t);
	task_set_result(t, task_await(t_result));
	return Task:0;
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
	return 1;
}