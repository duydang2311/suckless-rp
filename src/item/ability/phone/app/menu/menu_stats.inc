#if defined app_menu_stats_included
	#endinput
#endif

#define app_menu_stats_included

static Map:m_appStats;

#include <YSI_Coding\y_hooks>

hook IPApp_OnInitAttempt(id, app, Task:result) {
	if(app == AppMenu) {
		task_set_result(result, 1);
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
