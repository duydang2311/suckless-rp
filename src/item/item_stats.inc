#if defined item_stats_included
	#endinput
#endif

#define item_stats_included

const Float:MAX_ITEM_RELIABILITY = 1.0;

enum _:ItemAbility {
	iAbilityGun,
	iAbilityMelee,
	iAbilityBackpack
}

enum _:ItemStats {
	iAbility,
	iOccupiedSpace,
	Float:iReliability
}

static Map:m_itemStats;

stock Item_GetName(item_id, name[], size = sizeof(name)) {
	if(map_has_key(m_itemStats, item_id)) {
		new stats[ItemStats];
		map_get_arr(m_itemStats, item_id, stats);
		switch(stats[iAbility]) {
			case iAbilityGun: {
				IGun_GetName(item_id, name, size);
			}
			case iAbilityMelee: {
				// TODO: ItemMelee_GetName(item_id, name, size);
			}
		}
	}
	return 1;
}

stock Item_GetModel(item_id) {
	if(map_has_key(m_itemStats, item_id)) {
		new stats[ItemStats];
		map_get_arr(m_itemStats, item_id, stats);
		switch(stats[iAbility]) {
			case iAbilityGun: {
				return IGun_GetModel(item_id);
			}
			case iAbilityMelee: {
				// TODO: ItemMelee_GetModel(item_id);
			}
		}
	}
	return 0;
}

forward Item_OnCreated(item_id, ability, Float:reliability, occupied_space);
public Item_OnCreated(item_id, ability, Float:reliability, occupied_space) {
	new stats[ItemStats];
	stats[iAbility] = ability;
	stats[iReliability] = reliability;
	stats[iOccupiedSpace] = occupied_space;
	map_set_arr(m_itemStats, item_id, stats);
	return 1;
}

forward Item_OnPlayerEquipped(playerid, bool:hand_leftright, item_id);
public Item_OnPlayerEquipped(playerid, bool:hand_leftright, item_id) {
	if(map_has_key(m_itemStats, item_id)) {
		new stats[ItemStats];
		map_get_arr(m_itemStats, item_id, stats);
		switch(stats[iAbility]) {
			case iAbilityGun: {
				CallLocalFunction("IGun_OnPlayerEquipped", "iii", playerid, _:hand_leftright, item_id);
			}
			case iAbilityMelee: {
				CallLocalFunction("ItemMelee_OnPlayerEquipped", "iii", playerid, _:hand_leftright, item_id);
			}
		}
	}
	return 1;
}

forward Item_OnPlayerUnequipped(playerid, bool:hand_leftright, item_id);
public Item_OnPlayerUnequipped(playerid, bool:hand_leftright, item_id) {
	if(map_has_key(m_itemStats, item_id)) {
		new stats[ItemStats];
		map_get_arr(m_itemStats, item_id, stats);
		switch(stats[iAbility]) {
			case iAbilityGun: {
				CallLocalFunction("IGun_OnPlayerUnequipped", "iii", playerid, _:hand_leftright, item_id);
			}
			case iAbilityMelee: {
				CallLocalFunction("ItemMelee_OnPlayerUnequipped", "iii", playerid, _:hand_leftright, item_id);
			}
		}
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_itemStats = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_stats (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
	ability TINYINT UNSIGNED DEFAULT 0,\
	reliability FLOAT(11, 4) DEFAULT 0.0000,\
	occupied_space TINYINT UNSIGNED DEFAULT 0\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM item_stats", true);
	for(new i = 0, rows = cache_num_rows(), id, stats[ItemStats]; i != rows; i++) {
		cache_get_value_name_int(i, "id", id);
		cache_get_value_name_int(i, "ability", stats[iAbility]);
		cache_get_value_name_int(i, "occupied_space", stats[iOccupiedSpace]);
		cache_get_value_name_float(i, "reliablity", stats[iReliability]);
		map_set_arr(m_itemStats, id, stats);
	}
	cache_delete(result);
	return 1;
}