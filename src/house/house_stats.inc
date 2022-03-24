#if defined house_stats_included
	#endinput
#endif

#define house_stats_included

enum houseStats {
	houseOwnerId,
	houseDoorId
}

static Map:m_houseStats;

stock Task:House_Has(houseid) {
	new Task:t = task_new();

	new query[100];
	mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT COUNT(id) FROM house_stats WHERE id = %d LIMIT 1", houseid);
	mysql_pquery(MySQL_Connection(), query, "House_OnHas", "d", _:t);
	return t;
}

stock Task:House_Create() {
	new Task:t = task_new();

	new query[33 + 1];
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO house_stats VALUES ()");
	mysql_pquery(MySQL_Connection(), query, "House_OnCreated", "d", _:t);
	return t;
}

stock House_SetOwnerId(id, owner_id) {
	if(!map_has_key(m_houseStats, id)) {
		return 0;
	}
	new query[128];
	map_set_cell(m_houseStats, id, _:houseOwnerId, owner_id);
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE house_stats SET owner_id = %d WHERE id = %d LIMIT 1", owner_id, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock House_SetDoorId(id, door_id) {
	if(!map_has_key(m_houseStats, id)) {
		return 0;
	}
	new query[128];
	map_set_cell(m_houseStats, id, _:houseDoorId, door_id);
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE house_stats SET door_id = %d WHERE id = %d LIMIT 1", door_id, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock House_Delete(id) {
	if(!map_has_key(m_houseStats, id)) {
		return 0;
	}
	new query[128];
	map_remove(m_houseStats, id);
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM house_stats WHERE id = %d LIMIT 1", id);
	mysql_pquery(MySQL_Connection(), query);
	// CallLocalFunction("House_OnDeleted", "i", id); // if needed in future
	return 1;
}

forward House_OnCreated(Task:t);
public House_OnCreated(Task:t) {
	if(cache_affected_rows()) {
		new 
			stats[houseStats] = {-1, -1},
			id = cache_insert_id();
			
		map_set_arr(m_houseStats, id, stats);
		task_set_result(t, id);
		return 1;
	}
	
	task_set_result(t, -1);
	return 1;
}

forward House_OnHas(Task:t);
public House_OnHas(Task:t) {
	new count;
	cache_get_value_name_int(0, "COUNT(id)", count);

	if(count > 0) {
		task_set_result(t, 1);
		return 1;
	}

	task_set_result(t, 0);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_houseStats = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS house_stats (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
	owner_id INT UNSIGNED DEFAULT NULL,\
	door_id INT UNSIGNED DEFAULT NULL,\
	FOREIGN KEY (owner_id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE,\
	FOREIGN KEY (door_id) REFERENCES door_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM house_stats", true);

	new rows = cache_num_rows();
	for(new i = 0, stats[houseStats] = {-1, -1}, id, bool:null; i != rows; i++) {
		cache_get_value_name_int(i, "id", id);
		if(cache_is_value_name_null(i, "owner_id", null) && !null) {
			cache_get_value_name_int(i, "owner_id", stats[houseOwnerId]);
		}
		if(cache_is_value_name_null(i, "door_id", null) && !null) {
			cache_get_value_name_int(i, "door_id", stats[houseDoorId]);
		}
		map_set_arr(m_houseStats, id, stats);
	}
	cache_delete(result);
	return 1;
}

hook OnGameModeExit() {
	if(map_valid(m_houseStats)) {
		map_delete(m_houseStats);
		m_houseStats = Map:-1;
	}
	return 1;
}