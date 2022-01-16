static Map:m_adminFlags;

forward Admin_OnStatsRetrieved(playerid);
public Admin_OnStatsRetrieved(playerid) {
	if(cache_num_rows()) {
		new flags;
		cache_get_value_name_int(0, "flags", flags);
		if(!map_valid(m_adminFlags)) {
			m_adminFlags = map_new();
		}
		map_set(m_adminFlags, playerid, flags);
	}
	return 1;
}

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

hook Player_OnStatsRetrieved(playerid) {
	print("admin_stats.inc");
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT * FROM admin_flags WHERE id = %d LIMIT 1", Player_GetId(playerid));
	mysql_pquery(MySQL_Connection(), query, "Admin_OnStatsRetrieved", "i", playerid);
	return 1;
}

hook OnPlayerDisconnect(playerid) {
	if(map_valid(m_adminFlags) && map_has_key(m_adminFlags, playerid)) {
		map_remove(m_adminFlags, playerid);
		if(!map_size(m_adminFlags)) {
			map_delete(m_adminFlags);
		}
	}
	return 1;
}

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS admin_flags (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	flags TINYINT UNSIGNED DEFAULT 0,\
	FOREIGN KEY (id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}