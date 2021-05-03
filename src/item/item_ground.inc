#if defined item_ground_included
	#endinput
#endif

#define item_ground_included

enum _:ItemGroundStats {
	iGrObjectId,
	Text3D:iGrTextLabelId
}

static
	Map:m_iGroundStats,
	streamerExtraId;

stock Float:CompressRotation(Float:rotation){
	return (rotation - floatround(rotation/360.0,floatround_floor)*360.0);
}

static get_rotation_for_2point_3d(Float:x,Float:y,Float:z,Float:tx,Float:ty,Float:tz,&Float:rx,&Float:rz){
	new Float:radius = VectorSize(tx-x,ty-y,tz-z);
	if(radius <= 0.0) return 0;
	rx = CompressRotation(-(acos((tz-z)/radius)-90.0));
	rz = CompressRotation(atan2(ty-y,tx-x)-90.0);
	return 1;
}

static get_ground_rotation(Float:x,Float:y,Float:size,&Float:rx,&Float:ry){
	new Float:tpx1,Float:tpy1,Float:tpz1,
		Float:tpx2,Float:tpy2,Float:tpz2,
		Float:tmp;
	Util_GetPointInFront2D(x,y,0.0,size,tpx1,tpy1);
	Util_GetPointInFront2D(x,y,180.0,size,tpx2,tpy2);
	CA_FindZ_For2DCoord(tpx1,tpy1,tpz1);
	CA_FindZ_For2DCoord(tpx2,tpy2,tpz2);
	get_rotation_for_2point_3d(tpx1,tpy1,tpz1,tpx2,tpy2,tpz2,rx,tmp);

	Util_GetPointInFront2D(x,y,90.0,size,tpx1,tpy1);
	Util_GetPointInFront2D(x,y,270.0,size,tpx2,tpy2);
	CA_FindZ_For2DCoord(tpx1,tpy1,tpz1);
	CA_FindZ_For2DCoord(tpx2,tpy2,tpz2);
	get_rotation_for_2point_3d(tpx1,tpy1,tpz1,tpx2,tpy2,tpz2,ry,tmp);
}

stock IGround_Has(id) {
	return (map_valid(m_iGroundStats) && map_has_key(m_iGroundStats, id));
}

stock IGround_Update(id, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, int = 0, vw = 0) {
	if(!Item_Has(id)) {
		return 0;
	}

	new
		stats[ItemGroundStats] = {-1, Text3D:-1},
		name[64],
		query[512];
	if(!map_valid(m_iGroundStats)) {
		m_iGroundStats = map_new();
	} else if(map_has_key(m_iGroundStats, id)) {
		map_get_arr(m_iGroundStats, id, stats);
		if(stats[iGrObjectId] != -1) {
			DestroyDynamicObject(stats[iGrObjectId]);
		}
		if(stats[iGrTextLabelId] != Text3D:-1) {
			DestroyDynamic3DTextLabel(stats[iGrTextLabelId]);
		}
	}
	Item_GetName(id, name);
	stats[iGrObjectId] = CreateDynamicObject(Item_GetModel(id), x, y, z, rx, ry, rz, vw, int);
	stats[iGrTextLabelId] = CreateDynamic3DTextLabel(name, COLOR_YELLOW, x, y, z, 10.0, .worldid = vw, .interiorid = int);
	Streamer_SetIntData(STREAMER_TYPE_OBJECT, stats[iGrObjectId], E_STREAMER_EXTRA_ID, streamerExtraId);
	map_set_arr(m_iGroundStats, id, stats);
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_ground(id, pos_x, pos_y, pos_z, rot_x, rot_y, rot_z, interior, vw) VALUES (%d, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %d, %d) ON DUPLICATE KEY UPDATE pos_x = VALUES(pos_x), pos_y = VALUES(pos_y), pos_z = VALUES(pos_z), rot_x = VALUES(rot_x), rot_y = VALUES(rot_y), rot_z = VALUES(rot_z), interior = VALUES(interior), vw = VALUES(vw)", id, x, y, z, rx, ry, rz, int, vw);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock IGround_CAUpdate(id, Float:x, Float:y, Float:rz, int = 0, vw = 0, Float:dist_shift = 0.0) {
	if(!Item_Has(id)) {
		return 0;
	}

	new
		Float:tx,
		Float:ty,
		Float:z,
		Float:rx,
		Float:ry;
	Util_GetPointInFront2D(x, y, rz, dist_shift, tx, ty);
	get_ground_rotation(tx, ty, -1.5, rx, ry);
	CA_FindZ_For2DCoord(tx, ty, z);
	return IGround_Update(id, tx, ty, z, rx, ry, rz, int, vw);
}

stock IGround_Delete(id) {
	if(!map_valid(m_iGroundStats) || !map_has_key(m_iGroundStats, id)) {
		return 0;
	}
	new
		stats[ItemGroundStats],
		query[128];
	map_get_arr(m_iGroundStats, id, stats);
	DestroyDynamicObject(stats[iGrObjectId]);
	DestroyDynamic3DTextLabel(stats[iGrTextLabelId]);
	map_remove(m_iGroundStats, id);
	if(!map_size(m_iGroundStats)) {
		map_delete(m_iGroundStats);
		m_iGroundStats = Map:-1;
	}
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM item_ground WHERE id = %d LIMIT 1", id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock IGround_GetStreamerExtraId() {
	return streamerExtraId;
}

stock IGround_GetByObjectId(objectid) {
	for(new Iter:it = map_iter(m_iGroundStats), stats[ItemGroundStats]; iter_inside(it); iter_move_next(it)) {
		iter_get_arr(it, stats);
		if(stats[iGrObjectId] == objectid) {
			return iter_get_key(it);
		}
	}
	return -1;
}

hook OnGameModeExit@6() {
	if(map_valid(m_iGroundStats)) {
		map_delete(m_iGroundStats);
		m_iGroundStats = Map:-1;
	}
	return 1;
}

hook OnGameModeInit@6() {
	streamerExtraId = Session_GenerateId();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_ground (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	pos_x FLOAT (11, 4) DEFAULT 0.0000,\
	pos_y FLOAT (11, 4) DEFAULT 0.0000,\
	pos_z FLOAT (11, 4) DEFAULT 0.0000,\
	rot_x FLOAT (11, 4) DEFAULT 0.0000,\
	rot_y FLOAT (11, 4) DEFAULT 0.0000,\
	rot_z FLOAT (11, 4) DEFAULT 0.0000,\
	interior INT UNSIGNED DEFAULT 0,\
	vw INT UNSIGNED DEFAULT 0,\
	FOREIGN KEY (id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	mysql_query(MySQL_Connection(), "SELECT * FROM item_ground ground INNER JOIN item_stats stats ON ground.id = stats.id", true);
	new Cache:result = cache_save();
	for(new i = 0, rows = cache_num_rows(), igStats[ItemGroundStats], iStats[ItemStats], Float:pos[3], Float:rot[3], name[64], int, vw, id, Task:t; i != rows; i++) {
		cache_get_value_name_int(i, "id", id);
		cache_get_value_name_float(i, "pos_x", pos[0]);
		cache_get_value_name_float(i, "pos_y", pos[1]);
		cache_get_value_name_float(i, "pos_z", pos[2]);
		cache_get_value_name_float(i, "rot_x", rot[0]);
		cache_get_value_name_float(i, "rot_y", rot[1]);
		cache_get_value_name_float(i, "rot_z", rot[2]);
		cache_get_value_name_int(i, "interior", int);
		cache_get_value_name_int(i, "vw", vw);

		cache_get_value_name_int(i, "ability", iStats[iAbility]);
		cache_get_value_name_int(i, "occupied_space", iStats[iOccupiedSpace]);
		cache_get_value_name_float(i, "reliability", iStats[iReliability]);
		t = task_new();
		CallLocalFunction("Item_OnStatsRetrieving", "iifii", id, iStats[iAbility], iStats[iReliability], iStats[iOccupiedSpace], _:t);
		task_wait(t);
		cache_unset_active();
		cache_set_active(result);
		Item_GetName(id, name);
		igStats[iGrObjectId] = CreateDynamicObject(Item_GetModel(id), pos[0], pos[1], pos[2], rot[0], rot[1], rot[2], vw, int);
		igStats[iGrTextLabelId] = CreateDynamic3DTextLabel(name, COLOR_YELLOW, pos[0], pos[1], pos[2], 10.0, .worldid = vw, .interiorid = int);
		Streamer_SetIntData(STREAMER_TYPE_OBJECT, igStats[iGrObjectId], E_STREAMER_EXTRA_ID, streamerExtraId);
		if(!map_valid(m_iGroundStats)) {
			m_iGroundStats = map_new();
		}
		map_set_arr(m_iGroundStats, id, igStats);
	}
	cache_delete(result);
	return 1;
}
