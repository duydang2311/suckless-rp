#if defined item_melee_included
	#endinput
#endif

#define item_melee_included

enum _:ItemMeleeStats {
	Weapon:iMeleeWeapon,
	Float:iMeleeDamage,
	Float:iMeleeReliability,
}

static Map:m_meleeStats;

stock Task:IMelee_Create(Weapon:weapon, Float:damage) {
	new Task:t = task_new();
	task_detach();

	inline const QueryFinished() {
		new insert_id = -1;
		if(cache_affected_rows()) {
			new
				stats[ItemMeleeStats],
				query[150];

			mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_melee (id, weapon, damage) VALUES (%d, %d, %.4f)", insert_id, _:weapon, damage);
			mysql_pquery(MySQL_Connection(), query);
			stats[iMeleeWeapon] = weapon;
			stats[iMeleeDamage] = damage;
			stats[iMeleeReliability] = 1.0;
			if(!map_valid(m_meleeStats)) {
				m_meleeStats = map_new();
			}
			map_set_arr(m_meleeStats, insert_id, stats);
			CallLocalFunction("Item_OnCreated", "ii", insert_id, iAbilityMelee);
		}
		task_set_result(t, insert_id);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO item_stats (ability) VALUES (%d)", _:ITEM_ABILITY:iAbilityMelee);
	task_set_result_ms(t, -1, 2000);
	return t;
}

stock Weapon:IMelee_GetWeapon(item_id) {
	if(!map_valid(m_meleeStats) || !map_has_key(m_meleeStats, item_id)) {
		return Weapon:-1;
	}
	new stats[ItemMeleeStats];
	map_get_arr(m_meleeStats, item_id, stats);
	return stats[iMeleeWeapon];
}

stock IMelee_GetName(item_id, name[], size = sizeof(name)) {
	new Weapon:weapon = IMelee_GetWeapon(item_id);
	if(weapon == Weapon:-1) {
		return 0;
	}
	Weapon_GetName(weapon, name, size);
	return 1;
}

stock IMelee_GetModel(item_id) {
	new Weapon:weapon = IMelee_GetWeapon(item_id);
	if(weapon == Weapon:-1) {
		return 0;
	}
	return Weapon_GetModel(weapon);
}

forward IMelee_OnPlayerEquipped(playerid, bool:hand_leftright, item_id);
public IMelee_OnPlayerEquipped(playerid, bool:hand_leftright, item_id) {
	if(map_valid(m_meleeStats) && map_has_key(m_meleeStats, item_id)) {
		new stats[ItemGunStats];
		map_get_arr(m_meleeStats, item_id, stats);
		if(hand_leftright) {
			Weapon_SetPlayerArmed(playerid, stats[iMeleeWeapon], 1);
		}
	}
	return 1;
}

forward IMelee_OnPlayerUnequipped(playerid, bool:hand_leftright, item_id);
public IMelee_OnPlayerUnequipped(playerid, bool:hand_leftright, item_id) {
	if(hand_leftright) {
		new Weapon:weapon = IMelee_GetWeapon(item_id); // this function already checks for map valid
		if(weapon != Weapon:-1) {
			Weapon_Reset(playerid);
		}
	}
	return 1;
}

hook OnGameModeInit@3() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_melee (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	weapon TINYINT UNSIGNED DEFAULT 0,\
	damage FLOAT(11, 4) DEFAULT 0.0000,\
	reliability FLOAT(11, 4) DEFAULT 1.0000,\
	FOREIGN KEY (id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}

hook OnGameModeExit@3() {
	if(map_valid(m_meleeStats)) {
		map_delete_deep(m_meleeStats);
		m_meleeStats = Map:-1;
	}
	return 1;
}