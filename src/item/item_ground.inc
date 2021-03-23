#if defined item_ground_included
	#endinput
#endif

#define item_ground_included

enum _:ItemGroundStats {
	iGrObjectId,
	iGrTextLabelId
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_ground (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAM ON UPDATE CURRENT_TIMESTAMP,\
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

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM item_ground", true);
	for(new i = 0, rows = cache_num_rows(), stats[ItemGroundStats], Float:pos[3], Float:rot[3], name[64], int, vw, id; i != rows; i++) {
		cache_get_value_name_int(i, "id", id);
		cache_get_value_name_float(i, "pos_x", pos[0]);
		cache_get_value_name_float(i, "pos_y", pos[1]);
		cache_get_value_name_float(i, "pos_z", pos[2]);
		cache_get_value_name_float(i, "rot_x", rot[0]);
		cache_get_value_name_float(i, "rot_y", rot[1]);
		cache_get_value_name_float(i, "rot_z", rot[2]);
		cache_get_value_name_int(i, "interior", int);
		cache_get_value_name_int(i, "vw", vw);

		Item_GetName(id, name);
		stats[iGrObjectId] = CreateDynamicObject(Item_GetModel(id), pos[0], pos[1], pos[2], rot[0], rot[1], rot[2], vw, int);
		stats[iGrTextLabelId] = CreateDynamic3DTextLabel(name, COLOR_YELLOW, pos[0], pos[1], pos[2], 10.0, .worldid = vw, .interiorid = int);
		if(!map_valid(m_itemGroundStats)) {
			m_itemGroundStats = map_new();
		}
		map_set_arr(m_itemGroundStats, id, stats);
	}
	cache_delete(result);
	return 1;
}