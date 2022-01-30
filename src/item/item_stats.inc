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
	iAbilitySim,
	iAbilityRadio,
	iAbilitySize
}

enum _:ItemStats {
	iAbility,
	iOccupiedSpace,
	Float:iReliability
}

static Map:m_itemStats;

stock Task:Item_Has(id) {
	new
		Task:t = task_new(),
		query[128];
	mysql_format(MySQL_Connection(), query, sizeof query, "select count(*) from item_stats where id = %d limit 1", id);
	mysql_pquery(MySQL_Connection(), query, "Item_OnHas", "i", _:t);
	return t;
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
		case iAbilityRadio: {
			CallLocalFunction("IRadio_OnDeleted", "i", id);
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

stock Task:Item_GetOccupiedSpace(id) {
	new
		Task:t = task_new(),
		query[128];
	mysql_format(MySQL_Connection(), query, sizeof query, "select occupied_space from item_stats where id = %d limit 1", id);
	mysql_pquery(MySQL_Connection(), query, "Item_OnOccupiedSpaceGot", "i", _:t);
	return t;
}

stock Task:Item_GetAbility(id) {
	new
		Task:t = task_new(),
		query[128];
	mysql_format(MySQL_Connection(), query, sizeof query, "select ability from item_stats where id = %d limit 1", id);
	mysql_pquery(MySQL_Connection(), query, "Item_OnAbilityGot", "i", _:t);
	return t;
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

forward Item_OnHas(Task:t);
public Item_OnHas(Task:t) {
	new count;
	cache_get_value_name_int(0, "count(*)", count);
	task_set_result(t, count);
	return 1;
}

forward Item_OnOccupiedSpaceGot(Task:t);
public Item_OnOccupiedSpaceGot(Task:t) {
	new occupiedSpace = 0;
	if(cache_num_rows()) {
		cache_get_value_name_int(0, "occupied_space", occupiedSpace);
	}
	task_set_result(t, occupiedSpace);
	return 1;
}

forward Item_OnAbilityGot(Task:t);
public Item_OnAbilityGot(Task:t) {
	new ability = -1;
	if(cache_num_rows()) {
		cache_get_value_name_int(0, "ability", ability);
	}
	task_set_result(t, ability);
	return 1;
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

forward Item_OnStatsRetrieving(id, ability, Float:reliability, occupied_space, Task:t);
public Item_OnStatsRetrieving(id, ability, Float:reliability, occupied_space, Task:t) {
	Item_OnCreated(id, ability, reliability, occupied_space);
	CallLocalFunction("Item_OnStatsRetrieved", "ii", id, _:t);
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
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
	ability TINYINT UNSIGNED DEFAULT 0,\
	reliability FLOAT(11, 4) DEFAULT 0.0000,\
	occupied_space TINYINT UNSIGNED DEFAULT 0\
	)", false);
	return 1;
}

hook OnGameModeExit@0() {
	if(map_valid(m_itemStats)) {
		map_delete(m_itemStats);
		m_itemStats = Map:-1;
	}
	return 1;
}
