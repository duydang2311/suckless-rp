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
	bool:doorStreamingSync,
	doorPickupModel
}

enum doorAddons {
	doorDynEnAreaId,
	doorDynExAreaId,
	doorDynEnPickupId,
	doorDynExCPId
}

static Map:m_doorStats;
static Map:m_doorAddons;

stock Door_Valid(id) {
	return map_has_key(m_doorStats, id);
}

stock Task:Door_Create(DoorType:type = DOOR_TYPE_NONE) {
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
			map_set_arr(m_doorStats, insert_id, stats);
			task_set_result(t, insert_id);
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO door_stats (type) VALUES (%d)", type);
	return t;
}

stock Door_Delete(id) {
	if(!map_has_key(m_doorStats, id)) {
		return 0;
	}
	new string[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM door_stats WHERE id = %d LIMIT 1", id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetEntrance(id, Float:x, Float:y, Float:z, Float:a, int, vw) {
	if(!map_has_key(m_doorStats, id)) {
		return 0;
	}
	new query[256];
	Door_SetEnAddons(id, x, y, z, int, vw);
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE door_stats SET en_x = %.4f, en_y = %.4f, en_z = %.4f, en_a = %.4f, en_int = %d, en_vw = %d WHERE id = %d LIMIT 1", x, y, z, a, int, vw, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetExit(id, Float:x, Float:y, Float:z, Float:a, int, vw) {
	if(!map_has_key(m_doorStats, id)) {
		return 0;
	}
	new query[256];
	Door_SetExAddons(id, x, y, z, int, vw);
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE door_stats SET ex_x = %.4f, ex_y = %.4f, ex_z = %.4f, ex_a = %.4f, ex_int = %d, ex_vw = %d WHERE id = %d LIMIT 1", x, y, z, a, int, vw, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetEnInt(id, int) {
	if(!map_has_key(m_doorStats, id)) {
		return 0;
	}
	new query[128];
	Door_SetEnAddonsInt(id, int);
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE en_int = %d WHERE id = %d LIMIT 1", int, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetExInt(id, int) {
	if(!map_has_key(m_doorStats, id)) {
		return 0;
	}
	new query[128];
	Door_SetExAddonsInt(id, int);
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE ex_int = %d WHERE id = %d LIMIT 1", int, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetEnVW(id, vw) {
	if(!map_has_key(m_doorStats, id)) {
		return 0;
	}
	new query[128];
	Door_SetEnAddonsVW(id, vw);
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE en_vw = %d WHERE id = %d LIMIT 1", vw, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetExVW(id, vw) {
	if(!map_has_key(m_doorStats, id)) {
		return 0;
	}
	new query[128];
	Door_SetExAddonsVW(id, vw);
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE ex_vw = %d WHERE id = %d LIMIT 1", vw, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_GetLocked(id) {
	if(!map_has_key(m_doorStats, id)) {
		return 0;
	}
	new stats[doorStats];
	map_get_arr(m_doorStats, id, stats);
	return stats[doorLocked];
}

stock Door_GetVehicleAllowed(id) {
	if(!map_has_key(m_doorStats, id)) {
		return 0;
	}
	new stats[doorStats];
	map_get_arr(m_doorStats, id, stats);
	return stats[doorVehicleAllowed];
}

stock Door_GetStreamingSync(id) {
	if(!map_has_key(m_doorStats, id)) {
		return 0;
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

stock Door_SetEnAddons(id, Float:x, Float:y, Float:z, int, vw) {
	new
		pickupModel = 0,
		addon[doorAddons] = {-1, -1, -1, -1};
	if(map_has_key(m_doorAddons, id)) {
		new stats[doorStats];
		if(map_has_key(m_doorStats, id)) {
			map_get_arr(m_doorStats, id, stats);
			pickupModel = stats[doorPickupModel];
		}
		map_get_arr(m_doorAddons, id, addon);
		if(addon[doorDynEnAreaId] != -1) {
			DestroyDynamicArea(addon[doorDynEnAreaId]);
		}
		if(addon[doorDynEnPickupId] != -1) {
			DestroyDynamicPickup(addon[doorDynEnPickupId]);
		}
	}
	addon[doorDynEnAreaId] = CreateDynamicSphere(x, y, z, 2.5, vw, int),
	addon[doorDynEnPickupId] = CreateDynamicPickup(pickupModel, 0, x, y, z, vw, int, .streamdistance = 10.0);
	map_set_arr(m_doorAddons, id, addon);
	return 1;
}

stock Door_SetEnAddonsInt(id, int) {
	if(map_has_key(m_doorAddons, id)) {
		new addon[doorAddons] = {-1, -1, -1, -1};
		map_get_arr(m_doorAddons, id, addon);
		if(addon[doorDynEnAreaId] != -1) {
			Streamer_SetIntData(STREAMER_TYPE_AREA, addon[doorDynEnAreaId], E_STREAMER_INTERIOR_ID, int);
		}
		if(addon[doorDynEnPickupId] != -1) {
			Streamer_SetIntData(STREAMER_TYPE_PICKUP, addon[doorDynEnPickupId], E_STREAMER_INTERIOR_ID, int);
		}
	}
	return 1;
}

stock Door_SetEnAddonsVW(id, vw) {
	if(map_has_key(m_doorAddons, id)) {
		new addon[doorAddons] = {-1, -1, -1, -1};
		map_get_arr(m_doorAddons, id, addon);
		if(addon[doorDynEnAreaId] != -1) {
			Streamer_SetIntData(STREAMER_TYPE_AREA, addon[doorDynEnAreaId], E_STREAMER_WORLD_ID, vw);
		}
		if(addon[doorDynEnPickupId] != -1) {
			Streamer_SetIntData(STREAMER_TYPE_PICKUP, addon[doorDynEnPickupId], E_STREAMER_WORLD_ID, vw);
		}
	}
	return 1;
}

stock Door_SetExAddons(id, Float:x, Float:y, Float:z, int, vw) {
	new addon[doorAddons] = {-1, -1, -1, -1};
	if(map_has_key(m_doorAddons, id)) {
		map_get_arr(m_doorAddons, id, addon);
		if(addon[doorDynExAreaId] != -1) {
			DestroyDynamicArea(addon[doorDynExAreaId]);
		}
		if(addon[doorDynExCPId] != -1) {
			DestroyDynamicCP(addon[doorDynExCPId]);
		}
	}
	addon[doorDynExAreaId] = CreateDynamicSphere(x, y, z, 2.5, vw, int),
	addon[doorDynExCPId] = CreateDynamicCP(x, y, z, 10.0, vw, int, .streamdistance = 11.0);
	map_set_arr(m_doorAddons, id, addon);
	return 1;
}

stock Door_SetExAddonsInt(id, int) {
	if(map_has_key(m_doorAddons, id)) {
		new addon[doorAddons] = {-1, -1, -1, -1};
		map_get_arr(m_doorAddons, id, addon);
		if(addon[doorDynExAreaId] != -1) {
			Streamer_SetIntData(STREAMER_TYPE_AREA, addon[doorDynExAreaId], E_STREAMER_INTERIOR_ID, int);
		}
		if(addon[doorDynExCPId] != -1) {
			Streamer_SetIntData(STREAMER_TYPE_CP, addon[doorDynExCPId], E_STREAMER_INTERIOR_ID, int);
		}
	}
	return 1;
}

stock Door_SetExAddonsVW(id, vw) {
	if(map_has_key(m_doorAddons, id)) {
		new addon[doorAddons] = {-1, -1, -1, -1};
		map_get_arr(m_doorAddons, id, addon);
		if(addon[doorDynExAreaId] != -1) {
			Streamer_SetIntData(STREAMER_TYPE_AREA, addon[doorDynExAreaId], E_STREAMER_WORLD_ID, vw);
		}
		if(addon[doorDynExCPId] != -1) {
			Streamer_SetIntData(STREAMER_TYPE_CP, addon[doorDynExCPId], E_STREAMER_WORLD_ID, vw);
		}
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_doorStats = map_new();
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
			map_set_arr(m_doorStats, id, stats);
			Door_SetEnAddons(id, stats[doorEn][0], stats[doorEn][1], stats[doorEn][2], stats[doorEnInt], stats[doorEnVW]);
			Door_SetExAddons(id, stats[doorEx][0], stats[doorEx][1], stats[doorEx][2], stats[doorExInt], stats[doorExVW]);
		}
	}
	cache_delete(result);
	return 1;
}