#if defined item_ability_gun_included
	#endinput
#endif

#define item_ability_gun_included

enum _:ItemAbilityGunStats {
	Weapon:itemWeapon,
	itemAmmo,
	Float:itemDamage,
	Float:itemReliability,
}

static Map:m_weaponGun;

stock Weapon:ItemGun_GetWeapon(item_id) {
	if(map_has_key(m_weaponGun, item_id)) {
		new stats[ItemAbilityGunStats];
		map_get_arr(m_weaponGun, item_id, stats);
		return stats[itemWeapon];
	}
	return Weapon:-1;
}

stock Task:ItemGun_Create(Weapon:weapon, Float:damage, ammo = 0) {
	new Task:t = task_new();
	task_detach();

	inline const QueryFinished() {
		new
			insert_id = cache_insert_id(),
			stats[ItemAbilityGunStats],
			query[150];
		mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_ability_gun (id, weapon, ammo, damage) VALUES (%d, %d, %d, %.4f)", insert_id, _:weapon, ammo, damage);
		mysql_pquery(MySQL_Connection(), query);

		stats[itemWeapon] = weapon;
		stats[itemAmmo] = ammo;
		stats[itemDamage] = damage;
		stats[itemReliability] = 1.0;
		map_set_arr(m_weaponGun, insert_id, stats);
		task_set_result(t, insert_id);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO item_stats (ability) VALUES (%d)", _:ITEM_ABILITY:ITEM_ABILITY_GUN);
	return t;
}

forward ItemGun_OnPlayerEquipped(playerid, bool:hand_leftright, item_id);
public ItemGun_OnPlayerEquipped(playerid, bool:hand_leftright, item_id) {
	if(map_has_key(m_weaponGun, item_id)) {
		new stats[ItemAbilityGunStats];
		map_get_arr(m_weaponGun, item_id, stats);
		if(hand_leftright) {
			Weapon_SetPlayerArmed(playerid, stats[itemWeapon], stats[itemAmmo]);
		}
	}
	return 1;
}

forward ItemGun_OnPlayerUnequipped(playerid, bool:hand_leftright, item_id);
public ItemGun_OnPlayerUnequipped(playerid, bool:hand_leftright, item_id) {
	if(hand_leftright) {
		new Weapon:weapon = ItemGun_GetWeapon(item_id);
		if(weapon != Weapon:-1) {
			new
				ammo = Weapon_GetAmmo(playerid, weapon),
				query[128];
			mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE item_ability_gun SET ammo = %d WHERE id = %d LIMIT 1", ammo, item_id);
			mysql_pquery(MySQL_Connection(), query);
			print(query);
			Weapon_Reset(playerid);
		}
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_weaponGun = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_ability_gun (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	weapon TINYINT UNSIGNED DEFAULT 0,\
	ammo TINYINT UNSIGNED DEFAULT 0,\
	damage FLOAT(11, 4) DEFAULT 0.0000,\
	reliability FLOAT(11, 4) DEFAULT 1.0000,\
	FOREIGN KEY (id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}