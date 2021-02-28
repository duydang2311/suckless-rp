#if defined player_stats_included
	#endinput
#endif

#define player_stats_included

enum _:PlayerStats {
	pSkin,
	pLevel
}

static Map:m_playerStats;

stock Player_GetStats(playerid, stats[PlayerStats]) {
	if(map_has_key(m_playerStats, playerid)) {
		return map_get_arr_safe(m_playerStats, playerid, stats);
	}
	return 0;
}

stock Player_GetSkin(playerid) {
	if(map_has_key(m_playerStats, playerid)) {
		new stats[PlayerStats];
		map_get_arr(m_playerStats, playerid, stats);
		return stats[pSkin];
	}
	return -1;
}

stock Player_GetLevel(playerid) {
	if(map_has_key(m_playerStats, playerid)) {
		new stats[PlayerStats];
		map_get_arr(m_playerStats, playerid, stats);
		return stats[pLevel];
	}
	return -1;
}

#include <YSI_Coding\y_hooks>

hook Player_OnSpawn(playerid) {
	inline const QueryFinished() {
		if(cache_num_rows()) {
			new
				stats[PlayerStats],
				Float:health,
				Float:armour;
			cache_get_value_name_int(0, "skin", stats[pSkin]);
			cache_get_value_name_int(0, "level", stats[pLevel]);
			cache_get_value_name_float(0, "health", health);
			cache_get_value_name_float(0, "armour", armour);

			SetPlayerHealth(playerid, health);
			SetPlayerArmour(playerid, armour);
			SetPlayerSkin(playerid, stats[pSkin]);
			SetPlayerScore(playerid, stats[pLevel]);

			map_set_arr(m_playerStats, playerid, stats);
		} else {
			Player_KickMsg(playerid, "May chu ngat ket noi do xay ra loi trong luc tai du lieu nhan vat, vui long thong bao voi doi ngu quan tri.");
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT * FROM player_stats WHERE id = %d LIMIT 1", Player_GetId(playerid));
	return 1;
}

hook OnGameModeInit() {
	m_playerStats = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS player_stats (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	skin INT UNSIGNED DEFAULT 0,\
	health FLOAT(11, 4) DEFAULT 100.0000,\
	armour FLOAT(11, 4) DEFAULT 0.0000,\
	birthday TIMESTAMP DEFAULT 0,\
	level TINYINT UNSIGNED DEFAULT 1,\
	FOREIGN KEY (id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}