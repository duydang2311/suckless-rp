#if defined player_auth_included
	#endinput
#endif

#define player_auth_included

static Map:m_playerId;

stock Player_GetId(playerid) {
	if(map_has_key(m_playerId, playerid)) {
		return map_get(m_playerId, playerid);
	}
	return -1;
}

forward Player_OnLogin(playerid);
public Player_OnLogin(playerid) {
	new name[MAX_PLAYER_NAME + 1];
	GetPlayerName(playerid, name);

	inline const QueryFinished() {
		if(cache_num_rows()) {
			new id;
			cache_get_value_name_int(0, "id", id);
			map_set(m_playerId, playerid, id);

			Player_Spawn(playerid);
		} else {
			new query[128];

			Player_Msg(playerid, COLOR_LIGHTYELLOW, "Dang nhap khong thanh cong, xin hay thu lai.");
			GetPlayerName(playerid, name);
			mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT COUNT(*) FROM player_auth WHERE name = '%e'", name);
			mysql_pquery(MySQL_Connection(), query, "Player_OnAuthenticated", "i", playerid);
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT id FROM player_auth WHERE name = '%e' LIMIT 1", name);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS player_auth (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,\
	name VARCHAR(33) UNIQUE DEFAULT '',\
	hash VARCHAR(61) DEFAULT ''\
	);", false);
	return 1;
}