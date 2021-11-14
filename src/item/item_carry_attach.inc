#if defined item_carry_attach_included
	#endinput
#endif

#define item_carry_attach_included

stock Item_UpdateCarryAttachment(ability, Float:offset_x, Float:offset_y, Float:offset_z, Float:rot_x, Float:rot_y, Float:rot_z, Float:scale_x, Float:scale_y, Float:scale_z) {
	new query[1024];
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_carry_attach(ability, offset_x, offset_y, offset_z, rot_x, rot_y, rot_z, scale_x, scale_y, scale_z) VALUES (%d, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f) ON DUPLICATE KEY UPDATE offset_x = VALUES(offset_x), offset_y = VALUES(offset_y), offset_z = VALUES(offset_z), rot_x = VALUES(rot_x), rot_y = VALUES(rot_y), rot_z = VALUES(rot_z), scale_x = VALUES(scale_x), scale_y = VALUES(scale_y), scale_z = VALUES(scale_z)", ability, offset_x, offset_y, offset_z, rot_x, rot_y, rot_z, scale_x, scale_y, scale_z);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

hook OnGameModeInit@7() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_carry_attach (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
	ability TINYINT UNSIGNED PRIMARY KEY,\
	offset_x FLOAT(11, 4) DEFAULT 0.0000,\
	offset_y FLOAT(11, 4) DEFAULT 0.0000,\
	offset_z FLOAT(11, 4) DEFAULT 0.0000,\
	rot_x FLOAT(11, 4) DEFAULT 0.0000,\
	rot_y FLOAT(11, 4) DEFAULT 0.0000,\
	rot_z FLOAT(11, 4) DEFAULT 0.0000,\
	scale_x FLOAT(11, 4) DEFAULT 1.0000,\
	scale_y FLOAT(11, 4) DEFAULT 1.0000,\
	scale_z FLOAT(11, 4) DEFAULT 1.0000\
	)", false);
	return 1;
}