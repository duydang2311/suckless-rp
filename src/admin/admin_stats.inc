enum (<<= 1) {
	ADMIN_FLAGS_1 = 1,
	ADMIN_FLAGS_2,
	ADMIN_FLAGS_3,
	ADMIN_FLAGS_4,
	ADMIN_FLAGS_5
}

static Map:m_adminFlags;

stock Admin_GetFlags(playerid) {
	if(map_valid(m_adminFlags) && map_has_key(m_adminFlags, playerid)) {
		return map_get(m_adminFlags, playerid);
	}
	return 0;
}

stock Admin_SetFlags(playerid, flags) {
	new query[128];
	if(!flags) {
		mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM admin_flags WHERE id = %d LIMIT 1", Player_GetId(playerid));
	} else {
		mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO admin_flags (id, flags) VALUES (%d, %d) ON DUPLICATE KEY UPDATE flags = %d", Player_GetId(playerid), flags, flags);
	}
	if(!map_valid(m_adminFlags)) {
		m_adminFlags = map_new();
	}
	map_set(m_adminFlags, playerid, flags);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook Player_OnSpawn(playerid) {
	print("admin_stats.inc");
	inline const QueryFinished() {
		if(cache_num_rows()) {
			new flags;
			cache_get_value_name_int(0, "flags", flags);
			if(!map_valid(m_adminFlags)) {
				m_adminFlags = map_new();
			}
			map_set(m_adminFlags, playerid, flags);
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT * FROM admin_flags WHERE id = %d LIMIT 1", Player_GetId(playerid));
	return 1;
}

hook OnPlayerDisconnect(playerid) {
	if(map_valid(m_adminFlags) && map_has_key(m_adminFlags, playerid)) {
		map_remove(m_adminFlags, playerid);
	}
	return 1;
}

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS admin_flags (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	flags TINYINT UNSIGNED DEFAULT 0,\
	FOREIGN KEY (id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}