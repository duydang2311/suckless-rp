#if defined door_stats_included
	#endinput
#endif

#define door_stats_included

enum DOOR_TYPE {
	DOOR_TYPE_NONE
	DOOR_TYPE_HOUSE
	DOOR_TYPE_BUSINESS
}

enum doorStats {
	doorType,
	doorDesc[64],
	Float:doorEn[4],
	doorEnInt,
	doorEnVW,
	Float:doorEx[4],
	doorExInt,
	doorExVW,
	bool:doorLocked,
	bool:doorVehicleAllowed,
	bool:doorStreamingSync
}

static Map:m_door;

stock Door_Valid(id) {
	return map_has_key(m_door, id);
}

stock Task:Door_Create(type = DOOR_TYPE_NONE) {
	new Task:t = task_new();
	task_detach(t);

	inline const QueryFinished() {
		if(!cache_affected_rows()) {
			task_set_result(t, -1);
		} else {
			new
				insert_id = cache_insert_id(),
				stats[doorStats] = {0, "", {0.0000, 0.0000, 0.0000, 0.0000}, 0, 0, {0.0000, 0.0000, 0.0000, 0.0000}, 0, 0, false, false, false};
			stats[doorType] = type;
			map_set_arr(m_door, insert_id, stats);
			task_set_result(t, insert_id);
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO door_stats (type) VALUES (%d)", type);
	return t;
}

stock Door_Delete(id) {
	if(!map_has_key(m_door, id)) {
		return 0;
	}
	new string[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM door_stats WHERE id = %d LIMIT 1", id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetEntrance(id, Float:x, Float:y, Float:z, Float:a, int, vw) {
	if(!map_has_key(m_door, id)) {
		return 0;
	}
	new query[256];
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE door_stats SET en_x = %.4f, en_y = %.4f, en_z = %.4f, en_a = %.4f, en_int = %d, en_vw = %d WHERE id = %d LIMIT 1", x, y, z, a, int, vw, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetExit(id, Float:x, Float:y, Float:z, Float:a, int, vw) {
	if(!map_has_key(m_door, id)) {
		return 0;
	}
	new query[256];
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE door_stats SET ex_x = %.4f, ex_y = %.4f, ex_z = %.4f, ex_a = %.4f, ex_int = %d, ex_vw = %d WHERE id = %d LIMIT 1", x, y, z, a, int, vw, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetEnInt(id, int) {
	if(!map_has_key(m_door, id)) {
		return 0;
	}
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE en_int = %d WHERE id = %d LIMIT 1", int, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetExInt(id, int) {
	if(!map_has_key(m_door, id)) {
		return 0;
	}
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE ex_int = %d WHERE id = %d LIMIT 1", int, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetEnVW(id, vw) {
	if(!map_has_key(m_door, id)) {
		return 0;
	}
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE en_vw = %d WHERE id = %d LIMIT 1", vw, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetExVW(id, vw) {
	if(!map_has_key(m_door, id)) {
		return 0;
	}
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE ex_vw = %d WHERE id = %d LIMIT 1", vw, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_GetLocked(id) {
	if(!map_has_key(m_door, id)) {
		return 0;
	}
	new stats[doorStats];
	map_get_arr(m_door, id, stats);
	return stats[doorLocked];
}

stock Door_GetVehicleAllowed(id) {
	if(!map_has_key(m_door, id)) {
		return 0;
	}
	new stats[doorStats];
	map_get_arr(m_door, id, stats);
	return stats[doorVehicleAllowed];
}

stock Door_GetStreamingSync(id) {
	if(!map_has_key(m_door, id)) {
		return 0;
	}
	new stats[doorStats];
	map_get_arr(m_door, id, stats);
	return stats[doorStreamingSync];
}

stock Door_SetLocked(id, bool:locked) {
	if(!map_has_key(m_door, id) || Door_GetLocked(id) == locked) {
		return 0;
	}
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE door_stats SET locked = %d WHERE id = %d LIMIT 1", locked, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetVehicleAllowed(id, bool:allowed) {
	if(!map_has_key(m_door, id) || Door_GetVehicleAllowed(id) == allowed) {
		return 0;
	}
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE door_stats SET vehicle_allowed = %d WHERE id = %d LIMIT 1", allowed, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetStreamingSync(id, bool:sync) {
	if(!map_has_key(m_door, id) || Door_GetStreamingSync(id) == sync) {
		return 0;
	}
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE door_stats SET streaming_sync = %d WHERE id = %d LIMIT 1", sync, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_door = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS door_stats (\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
	type TINYINT UNSIGNED DEFAULT 0,\
	desc VARCHAR(64) DEFAULT '',\
	en_x FLOAT(11, 4) DEFAULT 0.0000,\
	en_y FLOAT(11, 4) DEFAULT 0.0000,\
	en_z FLOAT(11, 4) DEFAULT 0.0000,\
	en_a FLOAT(11, 4) DEFAULT 0.0000,\
	en_int INT UNSIGNED DEFAULT 0,\
	en_vw INT UNSIGNED DEFAULT 0,\
	ex_x FLOAT(11, 4) DEFAULT 0.0000,\
	ex_y FLOAT(11, 4) DEFAULT 0.0000,\
	ex_z FLOAT(11, 4) DEFAULT 0.0000,\
	ex_a FLOAT(11, 4) DEFAULT 0.0000,\
	ex_int INT UNSIGNED DEFAULT 0,\
	ex_vw INT UNSIGNED DEFAULT 0,\
	locked BOOLEAN DEFAULT FALSE,\
	vehicle_allowed BOOLEAN DEFAULT FALSE,\
	streaming_sync BOOLEAN DEFAULT FALSE\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM door_stats", true);

	new rows = cache_num_rows();
	if(rows) {
		new
			id,
			stats[doorStats];
		for(new i = 0; i != rows; i++) {
			cache_get_value_name_int(i, "id", id);
			cache_get_value_name(i, "desc", stats[doorDesc], 64);
			cache_get_value_name_float(i, "en_x", stats[doorEn][0]);
			cache_get_value_name_float(i, "en_y", stats[doorEn][1]);
			cache_get_value_name_float(i, "en_z", stats[doorEn][2]);
			cache_get_value_name_float(i, "en_a", stats[doorEn][3]);
			cache_get_value_name_int(i, "en_int", stats[doorEnInt]);
			cache_get_value_name_int(i, "en_vw", stats[doorEnVW]);
			cache_get_value_name_float(i, "ex_x", stats[doorEx][0]);
			cache_get_value_name_float(i, "ex_y", stats[doorEx][1]);
			cache_get_value_name_float(i, "ex_z", stats[doorEx][2]);
			cache_get_value_name_float(i, "ex_a", stats[doorEx][3]);
			cache_get_value_name_int(i, "ex_int", stats[doorExInt]);
			cache_get_value_name_int(i, "ex_vw", stats[doorExVW]);
			cache_get_value_name_int(i, "locked", stats[doorLocked]);
			cache_get_value_name_int(i, "vehicle_allowed", stats[doorVehicleAllowed]);
			cache_get_value_name_int(i, "streaming_sync", stats[doorStreamingSync]);
			map_set_arr(m_door, id, stats);
		}
	}
	cache_delete(result);
	return 1;
}