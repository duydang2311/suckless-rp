#if defined door_exit_included
	#endinput
#endif

#define door_exit_included

#include <YSI_Coding\y_unique>

const DoorExitExtraId = UNIQUE_SYMBOL;

enum _:DoorExit {
	Float:doorEx[4],
	doorExInt,
	doorExVW
}

enum _:DoorExitAddons {
	doorExCPId,
	doorExAreaId
}

static Map:m_doorExit,
	Map:m_doorExitAddons,
	doorExitExtraId;

stock Door_SetExit(id, Float:x, Float:y, Float:z, Float:a, int, vw) {
	if(!Door_Valid(id)) {
		return 0;
	}
	new
		query[384],
		ex[DoorExit];
	ex[doorEx][0] = x;
	ex[doorEx][1] = y;
	ex[doorEx][2] = z;
	ex[doorEx][3] = a;
	ex[doorExInt] = int;
	ex[doorExVW] = vw;
	map_set_arr(m_doorExit, id, ex);
	Door_UpdateExAddons(id);
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO door_exit (id, x, y, z, a, interior, vw) VALUES (%d, %.4f, %.4f, %.4f, %.4f, %d, %d) ON DUPLICATE KEY UPDATE x = VALUES(x), y = VALUES(y), z = VALUES(z), a = VALUES(a), interior = VALUES(interior), vw = VALUES(vw)", id, x, y, z, a, int, vw);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetExInt(id, int) {
	if(!map_valid(m_doorExit) || !map_has_key(m_doorExit, id)) {
		return 0;
	}
	new query[128];
	map_set_cell(m_doorExit, id, _:doorExInt, int);
	Door_UpdateExAddonsInt(id, int);
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE door_exit SET interior = %d WHERE id = %d LIMIT 1", int, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetExVW(id, vw) {
	if(!map_valid(m_doorExit) || !map_has_key(m_doorExit, id)) {
		return 0;
	}
	new query[128];
	map_set_cell(m_doorExit, id, _:doorExVW, vw);
	Door_UpdateExAddonsVW(id, vw);
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE door_exit SET vw = %d WHERE id = %d LIMIT 1", vw, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_UpdateExAddons(id) {
	if(!map_valid(m_doorExit) || !map_has_key(m_doorExit, id)) {
		return 0;
	}

	new
		ex[DoorExit],
		addon[DoorExitAddons] = {-1, -1};
	map_get_arr(m_doorExit, id, ex);
	if(map_valid(m_doorExitAddons) && map_has_key(m_doorExitAddons, id)) {
		map_get_arr(m_doorExitAddons, id, addon);
		if(addon[doorExAreaId] != -1) {
			DestroyDynamicArea(addon[doorExAreaId]);
		}
		if(addon[doorExCPId] != -1) {
			DestroyDynamicCP(addon[doorExCPId]);
		}
	}
	addon[doorExAreaId] = CreateDynamicSphere(ex[doorEx][0], ex[doorEx][1], ex[doorEx][2], 2.5, ex[doorExVW], ex[doorExInt]),
	addon[doorExCPId] = CreateDynamicCP(ex[doorEx][0], ex[doorEx][1], ex[doorEx][2], 2.5, ex[doorExVW], ex[doorExInt], .streamdistance = 10.0);
	Streamer_SetIntData(STREAMER_TYPE_AREA, addon[doorExAreaId], E_STREAMER_EXTRA_ID, doorExitExtraId);
	map_set_arr(m_doorExitAddons, id, addon);
	return 1;
}

stock Door_UpdateExAddonsInt(id, int) {
	if(map_valid(m_doorExitAddons) && map_has_key(m_doorExitAddons, id)) {
		new addon[DoorExitAddons];
		map_get_arr(m_doorExitAddons, id, addon);
		if(addon[doorExAreaId] != -1) {
			Streamer_SetIntData(STREAMER_TYPE_AREA, addon[doorExAreaId], E_STREAMER_INTERIOR_ID, int);
		}
		if(addon[doorExCPId] != -1) {
			Streamer_SetIntData(STREAMER_TYPE_CP, addon[doorExCPId], E_STREAMER_INTERIOR_ID, int);
		}
	}
	return 1;
}

stock Door_UpdateExAddonsVW(id, vw) {
	if(map_valid(m_doorExitAddons) && map_has_key(m_doorExitAddons, id)) {
		new addon[DoorExitAddons];
		map_get_arr(m_doorExitAddons, id, addon);
		if(addon[doorExAreaId] != -1) {
			Streamer_SetIntData(STREAMER_TYPE_AREA, addon[doorExAreaId], E_STREAMER_WORLD_ID, vw);
		}
		if(addon[doorExCPId] != -1) {
			Streamer_SetIntData(STREAMER_TYPE_CP, addon[doorExCPId], E_STREAMER_WORLD_ID, vw);
		}
	}
	return 1;
}

stock Door_StreamPlayerToExit(id, playerid) {
	if(!map_valid(m_doorExit) || !map_has_key(m_doorExit, id)) {
		return 0;
	}

	new ex[DoorExit];
	map_get_arr(m_doorExit, id, ex);
	if(Door_GetStreamingSync(id)) {
		Player_SyncedStreamToPosition(playerid, ex[doorEx][0], ex[doorEx][1], ex[doorEx][2], ex[doorEx][3], ex[doorExInt], ex[doorExVW]);
	} else {
		Player_StreamToPosition(playerid, ex[doorEx][0], ex[doorEx][1], ex[doorEx][2], ex[doorEx][3], ex[doorExInt], ex[doorExVW]);
	}
	return 1;
}

stock Door_GetIdByExitAreaId(areaid) {
	for(new Iter:it = map_iter(m_doorExitAddons), ex[DoorExitAddons]; iter_inside(it); iter_move_next(it)) {
		iter_get_arr(it, ex);
		if(ex[doorExAreaId] == areaid) {
			return iter_get_key(it);
		}
	}
	return -1;
}

#include <YSI_Coding\y_hooks>

hook Door_OnDeleted(id) {
	if(map_valid(m_doorExit) && map_has_key(m_doorExit, id)) {
		map_remove(m_doorExit, id);
	}
	if(map_valid(m_doorExitAddons) && map_has_key(m_doorExitAddons, id)) {
		new addon[DoorExitAddons];
		map_get_arr(m_doorExitAddons, id, addon);

		if(addon[doorExAreaId] != -1) {
			DestroyDynamicArea(addon[doorExAreaId]);
		}
		if(addon[doorExCPId] != -1) {
			DestroyDynamicCP(addon[doorExCPId]);
		}
		map_remove(m_doorExitAddons, id);
	}
	return 1;
}

hook OnGameModeInit() {
	m_doorExit = map_new();
	m_doorExitAddons = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS door_exit (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
	x FLOAT(11, 4) DEFAULT 0.0000,\
	y FLOAT(11, 4) DEFAULT 0.0000,\
	z FLOAT(11, 4) DEFAULT 0.0000,\
	a FLOAT(11, 4) DEFAULT 0.0000,\
	interior INT UNSIGNED DEFAULT 0,\
	vw INT UNSIGNED DEFAULT 0,\
	FOREIGN KEY (id) REFERENCES door_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM door_exit", true);

	new rows = cache_num_rows();
	if(rows) {
		new
			id,
			ex[DoorExit];
		for(new i = 0; i != rows; i++) {
			cache_get_value_name_int(i, "id", id);
			cache_get_value_name_float(i, "x", ex[doorEx][0]);
			cache_get_value_name_float(i, "y", ex[doorEx][1]);
			cache_get_value_name_float(i, "z", ex[doorEx][2]);
			cache_get_value_name_float(i, "a", ex[doorEx][3]);
			cache_get_value_name_int(i, "interior", ex[doorExInt]);
			cache_get_value_name_int(i, "vw", ex[doorExVW]);
			map_set_arr(m_doorExit, id, ex);
			Door_SetExit(id, ex[doorEx][0], ex[doorEx][1], ex[doorEx][2], ex[doorEx][3], ex[doorExInt], ex[doorExVW]);
		}
	}
	cache_delete(result);
	return 1;
}

hook OnGameModeExit() {
	if(map_valid(m_doorExit)) {
		map_delete(m_doorExit);
		m_doorExit = Map:-1;
	}
	if(map_valid(m_doorExitAddons)) {
		map_delete(m_doorExitAddons);
		m_doorExitAddons = Map:-1;
	}
	return 1;
}