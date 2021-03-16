#if defined door_type_house_included
	#endinput
#endif

#define door_type_house_included

static Map:m_doorHouseId;

stock Door_GetHouseId(id) {
	if(!map_has_key(m_doorHouseId, id)) {
		return -1;
	}
	return map_get(m_doorHouseId, id);
}

stock Door_SetHouseId(id, house_id) {
	if(Door_GetType(id) != DoorType:DOOR_TYPE_HOUSE) {
		return 0;
	}

	new query[150];
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO house_doors(door_id, house_id) VALUES (%d, %d) ON DUPLICATE KEY UPDATE house_id = VALUES(house_id)", id, house_id);
	mysql_pquery(MySQL_Connection(), query);
	map_set(m_doorHouseId, id, house_id);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_doorHouseId = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS house_doors (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	door_id INT UNSIGNED PRIMARY KEY,\
	house_id INT UNSIGNED,\
	FOREIGN KEY (door_id) REFERENCES door_stats(id) ON UPDATE CASCADE ON DELETE CASCADE,\
	FOREIGN KEY (house_id) REFERENCES house_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM house_doors", true);

	new rows = cache_num_rows();
	for(new i = 0, door_id, house_id; i != rows; i++) {
		cache_get_value_name_int(i, "door_id", door_id);
		cache_get_value_name_int(i, "house_id", house_id);
		map_set(m_doorHouseId, door_id, house_id);
	}
	cache_delete(result);
	return 1;
}
