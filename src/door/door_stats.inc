#if defined door_stats_included
	#endinput
#endif

#define door_stats_included

enum DoorType {
	DOOR_TYPE_NONE,
	DOOR_TYPE_HOUSE,
	DOOR_TYPE_BUSINESS
}

enum doorStats {
	DoorType:doorType,
	doorDesc[64],
	bool:doorLocked,
	bool:doorVehicleAllowed,
	bool:doorStreamingSync
}

static Map:m_doorStats;

stock Door_Valid(id) {
	return map_has_key(m_doorStats, id);
}

stock Task:Door_Create(DoorType:type = DOOR_TYPE_NONE) {
	new Task:t = task_new();
	task_detach();

	inline const QueryFinished() {
		if(!cache_affected_rows()) {
			task_set_result(t, -1);
		} else {
			new
				insert_id = cache_insert_id(),
				stats[doorStats] = {DoorType:DOOR_TYPE_NONE, "", false, false, false};
			stats[doorType] = type;
			map_set_arr(m_doorStats, insert_id, stats);
			task_set_result(t, insert_id);
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO door_stats (type) VALUES (%d)", _:type);
	return t;
}

stock Door_Delete(id) {
	if(!map_has_key(m_doorStats, id)) {
		return 0;
	}
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM door_stats WHERE id = %d LIMIT 1", id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock bool:Door_GetLocked(id) {
	if(!map_has_key(m_doorStats, id)) {
		return false;
	}
	new stats[doorStats];
	map_get_arr(m_doorStats, id, stats);
	return stats[doorLocked];
}

stock bool:Door_GetVehicleAllowed(id) {
	if(!map_has_key(m_doorStats, id)) {
		return false;
	}
	new stats[doorStats];
	map_get_arr(m_doorStats, id, stats);
	return stats[doorVehicleAllowed];
}

stock bool:Door_GetStreamingSync(id) {
	if(!map_has_key(m_doorStats, id)) {
		return false;
	}
	new stats[doorStats];
	map_get_arr(m_doorStats, id, stats);
	return stats[doorStreamingSync];
}

stock Door_SetLocked(id, bool:locked) {
	if(!map_has_key(m_doorStats, id) || Door_GetLocked(id) == locked) {
		return 0;
	}
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE door_stats SET locked = %d WHERE id = %d LIMIT 1", locked, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetVehicleAllowed(id, bool:allowed) {
	if(!map_has_key(m_doorStats, id) || Door_GetVehicleAllowed(id) == allowed) {
		return 0;
	}
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE door_stats SET vehicle_allowed = %d WHERE id = %d LIMIT 1", allowed, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetStreamingSync(id, bool:sync) {
	if(!map_has_key(m_doorStats, id) || Door_GetStreamingSync(id) == sync) {
		return 0;
	}
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE door_stats SET streaming_sync = %d WHERE id = %d LIMIT 1", sync, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_GetIdByAreaId(areaid) {
	new id = Door_GetIdByEntranceAreaId(areaid);
	if(id == -1) {
		id = Door_GetIdByExitAreaId(areaid);
	}
	return id;
}

stock DoorType:Door_GetType(id) {
	if(map_has_key(m_doorStats, id)) {
		new stats[doorStats];
		map_get_arr(m_doorStats, id, stats);
		return stats[doorType];
	}
	return -1;
}

stock Door_SetType(id, DoorType:type) {
	if(map_has_key(m_doorStats, id)) {
		new query[128];
		map_set_cell(m_doorStats, id, _:doorType, _:type);
		mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE door_stats SET type = %d WHERE id = %d LIMIT 1", _:type, id);
		mysql_pquery(MySQL_Connection(), query);
		return 1;
	}
	return 0;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_doorStats = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS door_stats (\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
	type TINYINT UNSIGNED DEFAULT 0,\
	description VARCHAR(64) DEFAULT '',\
	locked BOOLEAN DEFAULT FALSE,\
	vehicle_allowed BOOLEAN DEFAULT FALSE,\
	streaming_sync BOOLEAN DEFAULT FALSE\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM door_stats", true);

	new rows = cache_num_rows();
	if(rows) {
		new
			id,
			stats[doorStats],
			type;
		for(new i = 0; i != rows; i++) {
			cache_get_value_name_int(i, "id", id);
			cache_get_value_name_int(i, "type", type);
			stats[doorType] = DoorType:type;
			cache_get_value_name(i, "description", stats[doorDesc], 64);
			cache_get_value_name_int(i, "locked", stats[doorLocked]);
			cache_get_value_name_int(i, "vehicle_allowed", stats[doorVehicleAllowed]);
			cache_get_value_name_int(i, "streaming_sync", stats[doorStreamingSync]);
			map_set_arr(m_doorStats, id, stats);
		}
	}
	cache_delete(result);
	return 1;
}