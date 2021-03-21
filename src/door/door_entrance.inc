#if defined door_entrance_included
	#endinput
#endif

#define door_entrance_included

static doorEntranceExtraId = -1;

enum _:DoorEntrance {
	Float:doorEn[4],
	doorEnInt,
	doorEnVW,
	doorEnPickupModel
}

enum _:DoorEntranceAddons {
	doorEnPickupId,
	doorEnAreaId
}

static Map:m_doorEntrance,
	Map:m_doorEntranceAddons;

stock Door_SetEntrance(id, Float:x, Float:y, Float:z, Float:a, int, vw, pickupModel = 0) {
	if(!Door_Valid(id)) {
		return 0;
	}
	new
		query[384],
		entrance[DoorEntrance] = {{0.0, 0.0, 0.0, 0.0}, 0, 0, 0};
	if(map_has_key(m_doorEntrance, id)) {
		map_get_arr(m_doorEntrance, id, entrance);
	} else {
		entrance[doorEnPickupModel] = pickupModel;
	}
	entrance[doorEn][0] = x;
	entrance[doorEn][1] = y;
	entrance[doorEn][2] = z;
	entrance[doorEn][3] = a;
	entrance[doorEnInt] = int;
	entrance[doorEnVW] = vw;
	map_set_arr(m_doorEntrance, id, entrance);
	Door_UpdateEnAddons(id);
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO door_entrance (id, x, y, z, a, interior, vw, pickup_model) VALUES (%d, %.4f, %.4f, %.4f, %.4f, %d, %d, %d) ON DUPLICATE KEY UPDATE x = VALUES(x), y = VALUES(y), z = VALUES(z), a = VALUES(a), interior = VALUES(interior), vw = VALUES(vw), pickup_model = VALUES(pickup_model)", id, x, y, z, a, int, vw, pickupModel);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_UpdateEnAddons(id) {
	if(!map_has_key(m_doorEntrance, id)) {
		return 0;
	}

	new
		entrance[DoorEntrance],
		addon[DoorEntranceAddons] = {-1, -1};
	map_get_arr(m_doorEntrance, id, entrance);
	if(map_has_key(m_doorEntranceAddons, id)) {
		map_get_arr(m_doorEntranceAddons, id, addon);
		if(addon[doorEnAreaId] != -1) {
			DestroyDynamicArea(addon[doorEnAreaId]);
		}
		if(addon[doorEnPickupId] != -1) {
			DestroyDynamicPickup(addon[doorEnPickupId]);
		}
	}
	addon[doorEnAreaId] = CreateDynamicSphere(entrance[doorEn][0], entrance[doorEn][1], entrance[doorEn][2], 2.5, entrance[doorEnVW], entrance[doorEnInt]),
	addon[doorEnPickupId] = CreateDynamicPickup(entrance[doorEnPickupModel], 0, entrance[doorEn][0], entrance[doorEn][1], entrance[doorEn][2], entrance[doorEnVW], entrance[doorEnInt], .streamdistance = 10.0);
	Streamer_SetIntData(STREAMER_TYPE_AREA, addon[doorEnAreaId], E_STREAMER_EXTRA_ID, doorEntranceExtraId);
	map_set_arr(m_doorEntranceAddons, id, addon);
	return 1;
}

stock Door_SetEnInt(id, int) {
	if(!map_has_key(m_doorEntrance, id)) {
		return 0;
	}
	new query[128];
	map_set_cell(m_doorEntrance, id, _:doorEnInt, int);
	Door_UpdateEnAddonsInt(id, int);
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE door_entrance SET interior = %d WHERE id = %d LIMIT 1", int, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_SetEnVW(id, vw) {
	if(!map_has_key(m_doorEntrance, id)) {
		return 0;
	}
	new query[128];
	map_set_cell(m_doorEntrance, id, _:doorEnVW, vw);
	Door_UpdateEnAddonsVW(id, vw);
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE door_entrance SET vw = %d WHERE id = %d LIMIT 1", vw, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Door_UpdateEnAddonsInt(id, int) {
	if(map_has_key(m_doorEntranceAddons, id)) {
		new addon[DoorEntranceAddons];
		map_get_arr(m_doorEntranceAddons, id, addon);
		if(addon[doorEnAreaId] != -1) {
			Streamer_SetIntData(STREAMER_TYPE_AREA, addon[doorEnAreaId], E_STREAMER_INTERIOR_ID, int);
		}
		if(addon[doorEnPickupId] != -1) {
			Streamer_SetIntData(STREAMER_TYPE_PICKUP, addon[doorEnPickupId], E_STREAMER_INTERIOR_ID, int);
		}
	}
	return 1;
}

stock Door_UpdateEnAddonsVW(id, vw) {
	if(map_has_key(m_doorEntranceAddons, id)) {
		new addon[DoorEntranceAddons];
		map_get_arr(m_doorEntranceAddons, id, addon);
		if(addon[doorEnAreaId] != -1) {
			Streamer_SetIntData(STREAMER_TYPE_AREA, addon[doorEnAreaId], E_STREAMER_WORLD_ID, vw);
		}
		if(addon[doorEnPickupId] != -1) {
			Streamer_SetIntData(STREAMER_TYPE_PICKUP, addon[doorEnPickupId], E_STREAMER_WORLD_ID, vw);
		}
	}
	return 1;
}

stock Door_SetEnPickupModel(id, model) {
	if(map_has_key(m_doorEntrance, id)) {
		map_set_cell(m_doorEntrance, id, _:doorEnPickupModel, model);
		if(map_has_key(m_doorEntranceAddons, id)) {
			new addon[DoorEntranceAddons];
			map_get_arr(m_doorEntranceAddons, id, addon);
			if(addon[doorEnPickupId] != -1) {
				Streamer_SetIntData(STREAMER_TYPE_PICKUP, addon[doorEnPickupId], E_STREAMER_MODEL_ID, model);
			}
		}
		new query[128];
		mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE door_entrance SET pickup_model = %d WHERE id = %d LIMIT 1", model, id);
		mysql_pquery(MySQL_Connection(), query);
		return 1;
	}
	return 0;
}

stock Door_StreamPlayerToEntrance(id, playerid) {
	if(!map_has_key(m_doorEntrance, id)) {
		return 0;
	}

	new entrance[DoorEntrance];
	map_get_arr(m_doorEntrance, id, entrance);
	if(Door_GetStreamingSync(id)) {
		Player_SyncedStreamToPosition(playerid, entrance[doorEn][0], entrance[doorEn][1], entrance[doorEn][2], entrance[doorEn][3], entrance[doorEnInt], entrance[doorEnVW]);
	} else {
		Player_StreamToPosition(playerid, entrance[doorEn][0], entrance[doorEn][1], entrance[doorEn][2], entrance[doorEn][3], entrance[doorEnInt], entrance[doorEnVW]);
	}
	return 1;
}

stock Door_GetIdByEntranceAreaId(areaid) {
	for(new Iter:it = map_iter(m_doorEntranceAddons), entrance[DoorEntranceAddons]; iter_inside(it); iter_move_next(it)) {
		iter_get_arr(it, entrance);
		if(entrance[doorEnAreaId] == areaid) {
			return iter_get_key(it);
		}
	}
	return -1;
}

stock Door_GetEntranceExtraId() {
	return doorEntranceExtraId;
}

#include <YSI_Coding\y_hooks>

hook Door_OnDeleted(id) {
	if(map_has_key(m_doorEntrance, id)) {
		map_remove(m_doorEntrance, id);
	}
	if(map_has_key(m_doorEntranceAddons, id)) {
		new addon[DoorEntranceAddons];
		map_get_arr(m_doorEntranceAddons, id, addon);

		if(addon[doorEnAreaId] != -1) {
			DestroyDynamicArea(addon[doorEnAreaId]);
		}
		if(addon[doorEnPickupId] != -1) {
			DestroyDynamicPickup(addon[doorEnPickupId]);
		}
		map_remove(m_doorEntranceAddons, id);
	}
	return 1;
}

hook OnGameModeInit() {
	m_doorEntrance = map_new();
	m_doorEntranceAddons = map_new();
	doorEntranceExtraId = Session_GenerateId();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS door_entrance (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	pickup_model MEDIUMINT UNSIGNED DEFAULT 0,\
	x FLOAT(11, 4) DEFAULT 0.0000,\
	y FLOAT(11, 4) DEFAULT 0.0000,\
	z FLOAT(11, 4) DEFAULT 0.0000,\
	a FLOAT(11, 4) DEFAULT 0.0000,\
	interior INT UNSIGNED DEFAULT 0,\
	vw INT UNSIGNED DEFAULT 0,\
	FOREIGN KEY (id) REFERENCES door_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM door_entrance", true);

	new rows = cache_num_rows();
	if(rows) {
		new
			id,
			entrance[DoorEntrance];
		for(new i = 0; i != rows; i++) {
			cache_get_value_name_int(i, "id", id);
			cache_get_value_name_int(i, "pickup_model", entrance[doorEnPickupModel]);
			cache_get_value_name_float(i, "x", entrance[doorEn][0]);
			cache_get_value_name_float(i, "y", entrance[doorEn][1]);
			cache_get_value_name_float(i, "z", entrance[doorEn][2]);
			cache_get_value_name_float(i, "a", entrance[doorEn][3]);
			cache_get_value_name_int(i, "interior", entrance[doorEnInt]);
			cache_get_value_name_int(i, "vw", entrance[doorEnVW]);
			map_set_arr(m_doorEntrance, id, entrance);
			Door_SetEntrance(id, entrance[doorEn][0], entrance[doorEn][1], entrance[doorEn][2], entrance[doorEn][3], entrance[doorEnInt], entrance[doorEnVW], entrance[doorEnPickupModel]);
		}
	}
	cache_delete(result);
	return 1;
}

hook OnGameModeExit() {
	if(map_valid(m_doorEntrance)) {
		map_delete(m_doorEntrance);
		m_doorEntrance = Map:-1;
	}
	if(map_valid(m_doorEntranceAddons)) {
		map_delete(m_doorEntranceAddons);
		m_doorEntranceAddons = Map:-1;
	}
	return 1;
}