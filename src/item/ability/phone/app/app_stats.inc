#if defined app_stats_included
	#endinput
#endif

#define app_stats_included

const
	APP_NAME_SIZE = 32,
	APP_SPRITE_SIZE = 64;

enum _:App {
	AppMenu,
	AppStore,
	AppMessage,
	AppCall,
	AppSunGoPartner,
	AppSunGoDelivery,
	AppSunGoServer,
	AppSunGo,
}

enum _:AppStats {
	appName[APP_NAME_SIZE],
	appSprite[APP_SPRITE_SIZE],
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
	strcat(name, appStats[app][appName], size);
	return 1;
}

stock IPApp_GetSprite(app, sprite[], size = sizeof(sprite)) {
	if(!IPApp_Has(app)) {
		return 0;
	}

	sprite[0] = EOS;
	strcat(sprite, appStats[app][appSprite], size);
	return 1;
}

stock Task:IPApp_InitStatsAsync(id, app) {
	if(!IPhone_Has(id) || !IPApp_Has(app)) {
		return Task:0;
	}

	new
		Task:t = task_new(),
		Task:t_result = task_new();
	task_keep(t); // remember to manually delete the task by task_delete
	task_set_result_ms(t_result, 0, 2000);
	CallLocalFunction("IPA_OnStatsInitAttempt", "iii", id, app, _:t_result);
	task_detach();
	task_yield(t);
	new result = await t_result;
	task_set_result(t, result);
	task_delete(t); // manually delete the task because task_keep was used earlier
	return Task:0;
}

stock IPApp_InitStats(id, app) {
	if(!IPhone_Has(id) || !IPApp_Has(app)) {
		return 0;
	}

	CallLocalFunction("IPA_OnStatsInitAttempt", "iii", id, app, _:-1);
	return 1;
}

stock IPApp_ShowUIForPlayer(id, app, playerid) {
	if(!IPhone_Has(id) || !IPApp_Has(app)) {
		return 0;
	}
	print("IPApp_ShowUIForPlayer");
	CallLocalFunction("IPA_OnUIShowAttempt", "iii", id, app, playerid);
	return 1;
}

stock IPApp_TerminateStats(id, app) {
	if(!IPhone_Has(id) || !IPApp_Has(app)) {
		return 0;
	}

	printf("IPApp_TerminateStats id %d, app %d", id, app);
	CallLocalFunction("IPA_OnStatsTermnAttempt", "ii", id, app);
	return 1;
}

stock IPApp_HideUIForPlayer(id, playerid) {
	if(!IPhone_Has(id)) {
		return 0;
	}

	printf("IPApp_HideUIForPlayer id %d, playerid %d", id, playerid);
	CallLocalFunction("IPA_OnUIHideAttempt", "ii", id, playerid);
	return 1;
}

stock IPApp_GetActiveApps(id, bool:arr[], size = sizeof(arr)) {
	for(new i = 0; i != size; i++) {
		arr[i] = false;
	}
	CallLocalFunction("IPA_OnAppsGetAttempt", "iii", id, ref(arr), size);
	return 1;
}

stock IPApp_Ensure(app, const name[], const sprite[], bool:external = true) {
	new query[384];
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO app_stats (app, name, sprite, external) VALUES (%d, '%e', '%e', %d) ON DUPLICATE KEY UPDATE name = VALUES(name), sprite = VALUES(sprite), external = VALUES(external)", app, name, sprite, external);
	new Cache:result = mysql_query(MySQL_Connection(), query, true);
	if(cache_affected_rows()) {
		printf("INFO: Ensured phone application: {%d, '%s', '%s', '%s'}.", app, name, sprite, (external) ? "External" : "Built in");
	} else {
		printf("INFO: Ensured phone application: {%d, '%s', '%s', '%s'}, nothing to update.", app, name, sprite, (external) ? "External" : "Built in");
	}
	cache_delete(result); // set use_cache to true will perform blocking query? i guess
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPUI_OnShown@(id, playerid) {
	task_unblock(1);
	await IPApp_InitStatsAsync(id, AppMenu);
	IPApp_ShowUIForPlayer(id, AppMenu, playerid);
	return 1;
}

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS app_stats (\
		created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
		modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
		app TINYINT UNSIGNED PRIMARY KEY,\
		name VARCHAR(32) DEFAULT '',\
		sprite VARCHAR(64) DEFAULT '',\
		external BOOLEAN DEFAULT TRUE\
	)", false);

	CallLocalFunction("IPA_OnEnsureRequest", "");
	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM app_stats", true);
	new rows = cache_num_rows();
	for(new i = 0, app; i != rows; i++) {
		cache_get_value_name_int(i, "app", app);
		cache_get_value_name(i, "name", appStats[app][appName], APP_NAME_SIZE);
		cache_get_value_name(i, "sprite", appStats[app][appSprite], APP_SPRITE_SIZE);
		cache_get_value_name_int(i, "external", appStats[app][appExternal]);
	}
	cache_delete(result);
	printf("INFO: Loaded %d phone application stats.", rows);
	return 1;
}