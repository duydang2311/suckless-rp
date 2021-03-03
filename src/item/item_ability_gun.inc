#if defined item_ability_Gun_included
	#endinput
#endif

#define item_ability_Gun_included

enum ItemAbilityGunStats {
	Weapon:itemWeapon,
	itemName[33],
	Float:itemDamage,
	Float:itemReliability,
}

stock Task:Item_CreateGun(Weapon:weaponid, Float:damage, ammo = 0) {
	new Task:t = task_new();
	task_yield(1);

	inline const QueryFinished() {
		new
			insert_id = cache_insert_id(),
			query[150];
		mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_ability_gun (id, weaponId, name, ammo, damage) VALUES (%d, %d, '%e', %d, %.4f)", insert_id, model, name, ammo, damage);
		mysql_pquery(MySQL_Connection(), query);
		task_set_result(t, insert_id);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO item_stats (ability) VALUES (%d)", _:ItemAbility:ITEM_ABILITY_GUN);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_ability_gun (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	model MEDIUMINT DEFAULT 0,\
	name VARCHAR(33) DEFAULT '',\
	ammo TINYINT UNSIGNED DEFAULT 0,\
	damage FLOAT(11, 4) DEFAULT 0.0000,\
	reliability FLOAT(11, 4) DEFAULT 1.0000,\
	FOREIGN KEY (id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}