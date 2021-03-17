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

static Map:m_itemAbility;

stock Item_GetName(item_id, name[], size = sizeof(name)) {
	if(map_has_key(m_itemAbility, item_id)) {
		switch(ITEM_ABILITY:map_get(m_itemAbility, item_id)) {
			case ITEM_ABILITY_GUN: {
				ItemGun_GetName(item_id, name, size);
			}
			case ITEM_ABILITY_MELEE: {
				// TODO: ItemMelee_GetName(item_id, name, size);
			}
		}
	}
	return 1;
}

stock Item_GetModel(item_id) {
	if(map_has_key(m_itemAbility, item_id)) {
		switch(ITEM_ABILITY:map_get(m_itemAbility, item_id)) {
			case ITEM_ABILITY_GUN: {
				return ItemGun_GetModel(item_id);
			}
			case ITEM_ABILITY_MELEE: {
				// TODO: ItemMelee_GetModel(item_id);
			}
		}
	}
	return 0;
}

forward Item_OnCreated(item_id, ability);
public Item_OnCreated(item_id, ability) {
	map_set(m_itemAbility, item_id, ability);
	return 1;
}

forward Item_OnPlayerEquipped(playerid, bool:hand_leftright, item_id);
public Item_OnPlayerEquipped(playerid, bool:hand_leftright, item_id) {
	if(map_has_key(m_itemAbility, item_id)) {
		switch(ITEM_ABILITY:map_get(m_itemAbility, item_id)) {
			case ITEM_ABILITY_GUN: {
				print("call ItemGun_OnPlayerEquipped");
				CallLocalFunction("ItemGun_OnPlayerEquipped", "iii", playerid, _:hand_leftright, item_id);
			}
			case ITEM_ABILITY_MELEE: {
				CallLocalFunction("ItemMelee_OnPlayerEquipped", "iii", playerid, _:hand_leftright, item_id);
			}
		}
	}
	return 1;
}

forward Item_OnPlayerUnequipped(playerid, bool:hand_leftright, item_id);
public Item_OnPlayerUnequipped(playerid, bool:hand_leftright, item_id) {
	if(map_has_key(m_itemAbility, item_id)) {
		switch(ITEM_ABILITY:map_get(m_itemAbility, item_id)) {
			case ITEM_ABILITY_GUN: {
				CallLocalFunction("ItemGun_OnPlayerUnequipped", "iii", playerid, _:hand_leftright, item_id);
			}
			case ITEM_ABILITY_MELEE: {
				CallLocalFunction("ItemMelee_OnPlayerUnequipped", "iii", playerid, _:hand_leftright, item_id);
			}
		}
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_itemAbility = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_stats (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
	ability TINYINT UNSIGNED DEFAULT 0\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM item_stats", true);
	for(new i = 0, rows = cache_num_rows(), ability, id; i != rows; i++) {
		cache_get_value_name_int(i, "id", id);
		cache_get_value_name_int(i, "ability", ability);
		map_set(m_itemAbility, id, ability);
	}
	cache_delete(result);
	return 1;
}