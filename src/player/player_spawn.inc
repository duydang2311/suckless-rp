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
	print("player_spawn.inc");
	return 1;
}

stock Player_Spawn(playerid) {
	inline const QueryFinished() {
		new data[PlayerSpawn] = {0.0000, 0.0000, 2.0000, 0.0000, 0, 0};
		if(cache_num_rows()) {
			cache_get_value_name_float(0, "x", data[pSpawnX]);
			cache_get_value_name_float(0, "y", data[pSpawnY]);
			cache_get_value_name_float(0, "z", data[pSpawnZ]);
			cache_get_value_name_float(0, "a", data[pSpawnA]);
			cache_get_value_name_int(0, "vw", data[pSpawnVW]);
			cache_get_value_name_int(0, "interior", data[pSpawnInt]);
		} else {
			new query[150];
			mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO player_spawn (id,x,y,z,a,interior,vw) VALUES (%d,%.4f,%.4f,%.4f,%.4f,%d,%d)", Player_GetId(playerid), data[pSpawnX], data[pSpawnY], data[pSpawnZ], data[pSpawnA], data[pSpawnInt], data[pSpawnVW]);
			mysql_pquery(MySQL_Connection(), query);
		}
		TogglePlayerSpectating(playerid, false);
		SetSpawnInfo(playerid, 1, 0, data[pSpawnX], data[pSpawnY], data[pSpawnZ], data[pSpawnA], 0, 0, 0, 0, 0, 0);
		SpawnPlayer(playerid);
		SetPlayerVirtualWorld(playerid, data[pSpawnVW]);
		SetPlayerInterior(playerid, data[pSpawnInt]);
		TogglePlayerControllable(playerid, false);
		SetCameraBehindPlayer(playerid);

		SetPreciseTimer("Player_ToggleControllable", 1000, false, "ii", playerid, true);
		CallLocalFunction("Player_OnSpawn", "i", playerid);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT * FROM player_spawn WHERE id = %d LIMIT 1", Player_GetId(playerid));
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerDisconnect(playerid, reason) {
	new
		Float:x,
		Float:y,
		Float:z,
		Float:a,
		query[150];
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE player_spawn SET x = %.4f, y = %.4f, z = %.4f, a = %.4f, interior = %d, vw = %d WHERE id = %d LIMIT 1", x, y, z, a, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid), Player_GetId(playerid));
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

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
	interior INT UNSIGNED DEFAULT 0,\
	FOREIGN KEY (id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}