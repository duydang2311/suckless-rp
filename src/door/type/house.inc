#if defined door_type_house_included
	#endinput
#endif

#define door_type_house_included

stock Task:Door_GetHouseId(id) {
	if(Door_GetType(id) != DoorType:DOOR_TYPE_HOUSE) {
		return Task:0;
	}

	new Task:t = task_new();
	task_detach(t);

	inline const QueryFinish() {
		new house_id = -1;
		if(cache_num_rows()) {
			cache_get_value_name_int(0, "house_id", house_id);
		}
		task_set_result(t, house_id);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT house_id FROM house_doors WHERE door_id = %d LIMIT 1", id);
	return t;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS house_doors (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAM ON UPDATE CURRENT_TIMESTAMP,\
	door_id INT UNSIGNED PRIMARY KEY,\
	house_id INT UNSIGNED,\
	FOREIGN KEY (door_id) REFERENCES door_stats(id) ON UPDATE CASCADE ON DELETE CASCADE,\
	FOREIGN KEY (house_id) REFERENCES house_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}
