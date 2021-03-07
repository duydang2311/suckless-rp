#if defined item_stats_included
	#endinput
#endif

#define item_stats_included

const Float:MAX_ITEM_RELIABILITY = 1.0;

enum ITEM_ABILITY {
	ITEM_ABILITY_GUN,
	ITEM_ABILITY_MELEE,
	ITEM_ABILITY_PHONE,
	ITEM_ABILITY_FOOD
}

enum ItemStats {
	itemId,
	itemAbility
}

stock Item_GetName(item_id, name[], size = sizeof(name)) {
	inline const QueryFinished() {
		if(cache_num_rows()) {
			new ability;
			cache_get_value_name_int(0, "ability", ability);
			switch(ITEM_ABILITY:ability) {
				case ITEM_ABILITY_GUN: {
					ItemGun_GetName(item_id, name, size);
				}
				case ITEM_ABILITY_MELEE: {
					// TODO: ItemMelee_GetName(item_id, name, size);
				}
			}
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT ability FROM item_stats WHERE id = %d LIMIT 1", item_id);
	return 1;
}

forward Item_OnPlayerEquipped(playerid, bool:hand_leftright, item_id);
public Item_OnPlayerEquipped(playerid, bool:hand_leftright, item_id) {
	inline const QueryFinished() {
		if(cache_num_rows()) {
			new ability;
			cache_get_value_name_int(0, "ability", ability);
			switch(ITEM_ABILITY:ability) {
				case ITEM_ABILITY_GUN: {
					CallLocalFunction("ItemGun_OnPlayerEquipped", "iii", playerid, _:hand_leftright, item_id);
				}
				case ITEM_ABILITY_MELEE: {
					CallLocalFunction("ItemMelee_OnPlayerEquipped", "iii", playerid, _:hand_leftright, item_id);
				}
			}
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT ability FROM item_stats WHERE id = %d LIMIT 1", item_id);
	return 1;
}

forward Item_OnPlayerUnequipped(playerid, bool:hand_leftright, item_id);
public Item_OnPlayerUnequipped(playerid, bool:hand_leftright, item_id) {
	inline const QueryFinished() {
		if(cache_num_rows()) {
			new ability;
			cache_get_value_name_int(0, "ability", ability);
			switch(ITEM_ABILITY:ability) {
				case ITEM_ABILITY_GUN: {
					CallLocalFunction("ItemGun_OnPlayerUnequipped", "iii", playerid, _:hand_leftright, item_id);
				}
				case ITEM_ABILITY_MELEE: {
					CallLocalFunction("ItemMelee_OnPlayerUnequipped", "iii", playerid, _:hand_leftright, item_id);
				}
			}
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT ability FROM item_stats WHERE id = %d LIMIT 1", item_id);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_stats (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
	ability TINYINT UNSIGNED DEFAULT 0\
	)", false);
	return 1;
}