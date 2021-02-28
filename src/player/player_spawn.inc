#if defined player_spawn_included
	#endinput
#endif

#define player_spawn_included

enum PlayerSpawn {
	Float:pSpawnX,
	Float:pSpawnY,
	Float:pSpawnZ,
	Float:pSpawnA,
	pSpawnVW,
	pSpawnInt
}

forward Player_OnSpawn(playerid);
public Player_OnSpawn(playerid) {
	return 1;
}

stock Player_Spawn(playerid) {
	inline const QueryFinished() {
		new data[PlayerSpawn];
		cache_get_value_name_float(0, "x", data[pSpawnX]);
		cache_get_value_name_float(0, "y", data[pSpawnY]);
		cache_get_value_name_float(0, "z", data[pSpawnZ]);
		cache_get_value_name_float(0, "a", data[pSpawnA]);
		cache_get_value_name_int(0, "vw", data[pSpawnVW]);
		cache_get_value_name_int(0, "int", data[pSpawnInt]);
		SetSpawnInfo(playerid, 1, 0, data[pSpawnX], data[pSpawnY], data[pSpawnZ], data[pSpawnA], 0, 0, 0, 0, 0, 0);
		SpawnPlayer(playerid);
		SetPlayerVirtualWorld(playerid, data[pSpawnVW]);
		SetPlayerInterior(playerid, data[pSpawnInt]);
		TogglePlayerControllable(playerid, false);
		SetCameraBehindPlayer(playerid);

		Player_OnSpawn(playerid);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT * FROM player_spawn WHERE id = %d LIMIT 1", Player_GetId(playerid));
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS player_spawn (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	x FLOAT(11, 4) DEFAULT 0.0000,\
	y FLOAT(11, 4) DEFAULT 0.0000,\
	z FLOAT(11, 4) DEFAULT 0.0000,\
	a FLOAT(11, 4) DEFAULT 0.0000,\
	vw INT UNSIGNED DEFAULT 0,\
	int INT UNSIGNED DEFAULT 0,\
	FOREIGN KEY (id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}