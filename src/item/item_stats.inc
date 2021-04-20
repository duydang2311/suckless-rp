#if defined item_stats_included
	#endinput
#endif

#define item_stats_included

const Float:MAX_ITEM_RELIABILITY = 1.0;

enum _:ItemAbility {
	iAbilityGun,
	iAbilityMelee,
	iAbilityBackpack,
	iAbilityWhisk,
	iAbilityPhone,
	iAbilitySize
}

enum _:ItemStats {
	iAbility,
	iOccupiedSpace,
	Float:iReliability
}

static Map:m_itemStats;

stock Item_Has(id) {
	return (map_valid(m_itemStats) && map_has_key(m_itemStats, id));
}

stock Item_Delete(id) {
	if(!map_valid(m_itemStats) && map_has_key(m_itemStats, id)) {
		return 0;
	}
	new
		query[128],
		stats[ItemStats];
	map_get_arr(m_itemStats, id, stats);
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM item_stats WHERE id = %d LIMIT 1", id);
	mysql_pquery(MySQL_Connection(), query);
	switch(stats[iAbility]) {
		case iAbilityGun: {
			CallLocalFunction("IGun_OnDeleted", "i", id);
		}
		case iAbilityMelee: {
			// TODO: CallLocalFunction("IMelee_OnDeleted", "i", id);
		}
		case iAbilityBackpack: {
			CallLocalFunction("IBackpack_OnDeleted", "i", id);
		}
	}
	map_remove(m_itemStats, id);
	if(!map_size(m_itemStats)) {
		map_delete(m_itemStats);
		m_itemStats = Map:-1;
	}
	return 1;
}

stock Item_GetName(item_id, name[], size = sizeof(name)) {
	if(Item_Has(item_id)) {
		CallLocalFunction("Item_OnNameGetAttempt", "iii", item_id, ref(name), size);
	}
	return 1;
}

stock Item_GetModel(id) {
	if(map_valid(m_itemStats) && map_has_key(m_itemStats, id)) {
		new model = 0;
		CallLocalFunction("Item_OnModelGetAttempt", "ii", id, ref(model));
		return model;
	}
	return 0;
}

stock Item_GetCapacity(id) {
	if(!map_valid(m_itemStats) || !map_has_key(m_itemStats, id)) {
		return 0;
	}
	new stats[ItemStats];
	map_get_arr(m_itemStats, id, stats);
	switch(stats[iAbility]) {
		case iAbilityBackpack: {
			return IBackpack_GetCapacity(id);
		}
	}
	return 0;
}

stock Item_GetFreeCapacity(id) {
	if(!map_valid(m_itemStats) || !map_has_key(m_itemStats, id)) {
		return 0;
	}
	new stats[ItemStats];
	map_get_arr(m_itemStats, id, stats);
	switch(stats[iAbility]) {
		case iAbilityBackpack: {
			return IBackpack_GetFreeCapacity(id);
		}
	}
	return 0;
}

stock Item_GetOccupiedSpace(id) {
	if(!map_valid(m_itemStats) || !map_has_key(m_itemStats, id)) {
		return 0;
	}
	new stats[ItemStats];
	map_get_arr(m_itemStats, id, stats);
	return stats[iOccupiedSpace];
}

stock Item_GetAbility(id) {
	if(!map_valid(m_itemStats) || !map_has_key(m_itemStats, id)) {
		return 0;
	}
	new stats[ItemStats];
	map_get_arr(m_itemStats, id, stats);
	return stats[iAbility];
}

stock Item_HasAbility(ability) {
	return (ability >= 0 && ability < iAbilitySize);
}

stock Item_GetModelByAbility(ability) {
	switch(ability) {
		case iAbilityWhisk: {
			return IWhisk_GetModel();
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
	if(!map_valid(m_itemStats)) {
		m_itemStats = map_new();
	}
	map_set_arr(m_itemStats, item_id, stats);
	return 1;
}

forward Item_OnPlayerEquipped(playerid, bool:hand_leftright, item_id);
public Item_OnPlayerEquipped(playerid, bool:hand_leftright, item_id) {
	if(map_valid(m_itemStats) && map_has_key(m_itemStats, item_id)) {
		new stats[ItemStats];
		map_get_arr(m_itemStats, item_id, stats);
		switch(stats[iAbility]) {
			case iAbilityGun: {
				CallLocalFunction("IGun_OnPlayerEquipped", "iii", playerid, _:hand_leftright, item_id);
			}
			case iAbilityMelee: {
				CallLocalFunction("IMelee_OnPlayerEquipped", "iii", playerid, _:hand_leftright, item_id);
			}
		}
	}
	return 1;
}

forward Item_OnPlayerUnequipped(playerid, bool:hand_leftright, item_id);
public Item_OnPlayerUnequipped(playerid, bool:hand_leftright, item_id) {
	if(map_valid(m_itemStats) && map_has_key(m_itemStats, item_id)) {
		new stats[ItemStats];
		map_get_arr(m_itemStats, item_id, stats);
		switch(stats[iAbility]) {
			case iAbilityGun: {
				CallLocalFunction("IGun_OnPlayerUnequipped", "iii", playerid, _:hand_leftright, item_id);
			}
			case iAbilityMelee: {
				CallLocalFunction("IMelee_OnPlayerUnequipped", "iii", playerid, _:hand_leftright, item_id);
			}
		}
	}
	return 1;
}

hook OnGameModeInit@0() {
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
		cache_get_value_name_float(i, "reliability", stats[iReliability]);
		if(!map_valid(m_itemStats)) {
			m_itemStats = map_new();
		}
		map_set_arr(m_itemStats, id, stats);
	}
	cache_delete(result);
	return 1;
}

hook OnGameModeExit@0() {
	if(map_valid(m_itemStats)) {
		map_delete(m_itemStats);
		m_itemStats = Map:-1;
	}
	return 1;
}