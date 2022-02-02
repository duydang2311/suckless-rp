#if defined item_gun_included
	#endinput
#endif

#define item_gun_included

enum _:ItemGunStats {
	Weapon:iGunWeapon,
	Float:iGunDamage
}

static Map:m_gunStats;

stock Task:IGun_GetWeapon(item_id) {
	new
		Task:t = task_new(),
		query[128];
	mysql_format(MySQL_Connection(), query, sizeof query, "select weapon from item_gun where id = %d limit 1", item_id);
	mysql_pquery(MySQL_Connection(), query, "IGun_OnWeaponGot", "i", _:t);
	return t;
}

stock Task:IGun_Create(Weapon:weapon, Float:damage, stock_ammo = 0, Float:reliability = 1.0, occupied_space = 1) {
	new Task:t = task_new();
	task_detach();

	inline const QueryFinished() {
		new insert_id = -1;
		if(cache_affected_rows()) {
			new
				stats[ItemGunStats],
				query[150];
			insert_id = cache_insert_id();
			mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_gun (id, weapon, ammo, damage) VALUES (%d, %d, %d, %.4f)", insert_id, _:weapon, stock_ammo, damage);
			mysql_pquery(MySQL_Connection(), query);

			stats[iGunWeapon] = weapon;
			stats[iGunDamage] = damage;
			if(!map_valid(m_gunStats)) {
				m_gunStats = map_new();
			}
			map_set_arr(m_gunStats, insert_id, stats);
			CallLocalFunction("Item_OnCreated", "iifi", insert_id, iAbilityGun, reliability, occupied_space);
		}
		task_set_result(t, insert_id);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO item_stats (ability, reliability, occupied_space) VALUES (%d, %.4f, %d)", iAbilityGun, reliability, occupied_space);
	task_set_result_ms(t, -1, 2000);
	return t;
}

stock Task:IGun_GetModel(item_id) {
	new
		Task:t = task_new(),
		query[128];
	mysql_format(MySQL_Connection(), query, sizeof query, "select weapon from item_gun where id = %d limit 1", item_id);
	mysql_pquery(MySQL_Connection(), query, "IGun_OnModelGot", "i", _:t);
	return t;
}

forward IGun_OnWeaponGot(Task:task);
public IGun_OnWeaponGot(Task:task) {
	new weapon = -1;
	if(cache_num_rows()) {
		cache_get_value_name_int(0, "weapon", weapon);
	}
	task_set_result(task, weapon);
	return 1;
}

forward IGun_OnModelGot(Task:task);
public IGun_OnModelGot(Task:task) {
	if(cache_num_rows()) {
		new weapon;
		cache_get_value_name_int(0, "weapon", weapon);
		task_set_result(task, Weapon_GetModel(Weapon:weapon));
		return 1;
	}
	task_set_result(task, 0);
	return 1;
}

forward IGun_OnDeleted(id);
public IGun_OnDeleted(id) {
	if(map_valid(m_gunStats) && map_has_key(m_gunStats, id)) {
		map_remove(m_gunStats, id);
		if(!map_size(m_gunStats)) {
			map_delete(m_gunStats);
			m_gunStats = Map:-1;
		}
	}
	return 1;
}

forward IGun_OnPlayerEquipped(playerid, bool:hand_leftright, item_id);
public IGun_OnPlayerEquipped(playerid, bool:hand_leftright, item_id) {
	if(map_valid(m_gunStats) && map_has_key(m_gunStats, item_id)) {
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
		task_unblock(1);
		new weapon = await IGun_GetWeapon(item_id);
		if(weapon != 1) {
			new
				ammo = Weapon_GetAmmo(playerid, Weapon:weapon),
				query[128];
			mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE item_gun SET ammo = %d WHERE id = %d LIMIT 1", ammo, item_id);
			mysql_pquery(MySQL_Connection(), query);
			Weapon_Reset(playerid);
		}
	}
	return 1;
}

hook OnGameModeInit@2() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_gun (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	weapon TINYINT UNSIGNED DEFAULT 0,\
	ammo TINYINT UNSIGNED DEFAULT 0,\
	damage FLOAT(11, 4) DEFAULT 0.0000,\
	reliability FLOAT(11, 4) DEFAULT 1.0000,\
	FOREIGN KEY (id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new query[150];
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM item_stats WHERE NOT EXISTS ( SELECT * FROM item_gun WHERE item_stats.id = item_gun.id ) AND item_stats.ability = %d", iAbilityGun);
	new Cache:result = mysql_query(MySQL_Connection(), query, true);
	if(cache_affected_rows()) {
		printf("INFO: Deleted %d rows with non-existing referenced `id` to item_gun in item_stats.", cache_affected_rows());
	}
	cache_delete(result);
	return 1;
}

hook OnGameModeExit@2() {
	if(map_valid(m_gunStats)) {
		map_delete(m_gunStats);
		m_gunStats = Map:-1;
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

forward IGun_OnStatsRetrieving(id, Task:t);
public IGun_OnStatsRetrieving(id, Task:t) {
	if(cache_num_rows()) {
		new
			iWeapon,
			stats[ItemGunStats];
		cache_get_value_name_int(0, "weapon", iWeapon);
		cache_get_value_name_float(0, "damage", stats[iGunDamage]);
		stats[iGunWeapon] = Weapon:iWeapon;
		if(!map_valid(m_gunStats)) {
			m_gunStats = map_new();
		}
		map_set_arr(m_gunStats, id, stats);
	}
	if(task_valid(t)) {
		task_set_result(t, 1);
	}
	return 1;
}

hook Item_OnStatsRetrieved(id, Task:t) {
	if(task_await(Item_GetAbility(id)) == iAbilityGun) {
		new query[128];
		mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT * FROM item_gun WHERE id = %d LIMIT 1", id);
		mysql_pquery(MySQL_Connection(), query, "IGun_OnStatsRetrieving", "ii", id, _:t);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook Item_OnModelGetAttempt(id, &model) {
	if(task_await(Item_GetAbility(id)) == iAbilityGun) {
		model = IGun_GetModel(id);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook Item_OnNameGet(id, ability, Task:task) {
	if(ability == iAbilityGun) {
		task_unblock(Y_HOOKS_BREAK_RETURN_1);
		new weapon = await IGun_GetWeapon(id);
		new name[WEAPON_NAME_LENGTH] = "";
		if(weapon != 1) {
			Weapon_GetName(Weapon:weapon, name);
		}
		task_set_result_str(task, name);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}
