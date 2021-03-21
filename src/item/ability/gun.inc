#if defined item_gun_included
	#endinput
#endif

#define item_gun_included

enum _:ItemGunStats {
	Weapon:iGunWeapon,
	Float:iGunDamage
}

static Map:m_gunStats;

stock Weapon:IGun_GetWeapon(item_id) {
	if(map_has_key(m_gunStats, item_id)) {
		new stats[ItemGunStats];
		map_get_arr(m_gunStats, item_id, stats);
		return stats[iGunWeapon];
	}
	return Weapon:-1;
}

stock Task:IGun_Create(Weapon:weapon, Float:damage, stock_ammo = 0, Float:reliability = 1.0, occupied_space = 1) {
	new Task:t = task_new();
	task_detach();

	inline const QueryFinished() {
		new
			insert_id = cache_insert_id(),
			stats[ItemGunStats],
			query[150];
		mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_gun (id, weapon, ammo, damage) VALUES (%d, %d, %d, %.4f)", insert_id, _:weapon, stock_ammo, damage);
		mysql_pquery(MySQL_Connection(), query);

		stats[iGunWeapon] = weapon;
		stats[iGunDamage] = damage;
		if(!map_valid(m_gunStats)) {
			m_gunStats = map_new();
		}
		map_set_arr(m_gunStats, insert_id, stats);
		CallLocalFunction("Item_OnCreated", "iifi", insert_id, iAbilityGun, reliability, occupied_space);
		task_set_result(t, insert_id);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO item_stats (ability, reliability, occupied_space) VALUES (%d, %.4f, %d)", iAbilityGun, reliability, occupied_space);
	return t;
}

stock IGun_GetName(item_id, name[], size = sizeof(name)) {
	new Weapon:weapon = IGun_GetWeapon(item_id);
	if(weapon == Weapon:-1) {
		return 0;
	}
	Weapon_GetName(weapon, name, size);
	return 1;
}

stock IGun_GetModel(item_id) {
	new Weapon:weapon = IGun_GetWeapon(item_id);
	if(weapon == Weapon:-1) {
		return 0;
	}
	return Weapon_GetModel(weapon);
}

forward IGun_OnDeleted(id);
public IGun_OnDeleted(id) {
	if(map_has_key(m_gunStats, id)) {
		map_remove(m_gunStats, id);
	}
	return 1;
}

forward IGun_OnPlayerEquipped(playerid, bool:hand_leftright, item_id);
public IGun_OnPlayerEquipped(playerid, bool:hand_leftright, item_id) {
	if(map_has_key(m_gunStats, item_id)) {
		new stats[ItemGunStats];
		map_get_arr(m_gunStats, item_id, stats);
		if(hand_leftright) {
			inline const QueryFinished() {
				if(cache_num_rows()) {
					new ammo;
					cache_get_value_name_int(0, "ammo", ammo);
					Weapon_SetPlayerArmed(playerid, stats[iGunWeapon], ammo);
				}
			}
			MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT ammo FROM item_gun WHERE id = %d LIMIT 1", item_id);
		}
	}
	return 1;
}

forward IGun_OnPlayerUnequipped(playerid, bool:hand_leftright, item_id);
public IGun_OnPlayerUnequipped(playerid, bool:hand_leftright, item_id) {
	if(hand_leftright) {
		new Weapon:weapon = IGun_GetWeapon(item_id); // this function already checks for map valid
		if(weapon != Weapon:-1) {
			new
				ammo = Weapon_GetAmmo(playerid, weapon),
				query[128];
			mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE item_gun SET ammo = %d WHERE id = %d LIMIT 1", ammo, item_id);
			mysql_pquery(MySQL_Connection(), query);
			Weapon_Reset(playerid);
		}
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_gunStats = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_gun (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	weapon TINYINT UNSIGNED DEFAULT 0,\
	ammo TINYINT UNSIGNED DEFAULT 0,\
	damage FLOAT(11, 4) DEFAULT 0.0000,\
	reliability FLOAT(11, 4) DEFAULT 1.0000,\
	FOREIGN KEY (id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM item_gun", true);

	new rows = cache_num_rows();
	if(rows) {
		new
			iWeapon,
			item_id;
		for(new i = 0; i != rows; i++) {
			new stats[ItemGunStats];
			cache_get_value_name_int(i, "id", item_id);
			cache_get_value_name_int(i, "weapon", iWeapon);
			cache_get_value_name_float(i, "damage", stats[iGunDamage]);
			stats[iGunWeapon] = Weapon:iWeapon;

			map_set_arr(m_gunStats, item_id, stats);
		}
	}
	cache_delete(result);
	printf("INFO: Loaded %d item with gun ability.", rows);
	return 1;
}

hook OnGameModeExit() {
	if(map_valid(m_gunStats)) {
		map_delete(m_gunStats);
		m_gunStats = Map:-1;
	}
	return 1;
}