#if defined door_stats_included
	#endinput
#endif

#define door_stats_included

enum doorStats {
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

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_door = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS door_stats (\
	id INT UNSIGNED PRIMARY KEY,\
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
	streaming_sync BOOLEAN DEFAULT FALSE,\
	FOREIGN KEY (id) REFERENCES door(id) ON UPDATE CASCADE ON DELETE CASCADE,\
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