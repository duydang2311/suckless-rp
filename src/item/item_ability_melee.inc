#if defined item_ability_melee_included
	#endinput
#endif

#define item_ability_melee_included

enum ItemAbilityMeleeStats {
	Weapon:itemWeapon,
	Float:itemDamage,
	Float:itemReliability,
}

static Map:m_weaponMelee;

stock Task:Item_CreateMelee(Weapon:weapon, Float:damage) {
	new Task:t = task_new();
	task_detach();

	inline const QueryFinished() {
		new
			insert_id = cache_insert_id(),
			stats[ItemAbilityGunStats],
			name[32 + 1],
			query[150];

		mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_ability_gun (id, weapon, damage) VALUES (%d, %d, %.4f)", insert_id, model, damage);
		mysql_pquery(MySQL_Connection(), query);
		stats[itemWeapon] = weapon;
		stats[itemDamage] = damage;
		stats[itemReliability] = 1.0;
		map_set(m_weaponMelee, insert_id, stats);
		task_set_result(t, insert_id);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO item_stats (ability) VALUES (%d)", _:ITEM_ABILITY:ITEM_ABILITY_MELEE);
	return t;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_weaponMelee = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_ability_melee (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	weapon TINYINT UNSIGNED DEFAULT 0,\
	damage FLOAT(11, 4) DEFAULT 0.0000,\
	reliability FLOAT(11, 4) DEFAULT 1.0000,\
	FOREIGN KEY (id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}