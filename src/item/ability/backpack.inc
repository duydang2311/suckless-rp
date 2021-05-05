#if defined item_backpack_included
	#endinput
#endif

#define item_backpack_included

enum _:ItemBackpackStats {
	iBpModel,
	iBpCapacity
}

static Map:m_backpackStats;

stock IBackpack_Has(id) {
	return (map_valid(m_backpackStats) && map_has_key(m_backpackStats, id));
}

stock Task:IBackpack_Create(model, capacity, Float:reliability = 1.0, occupied_space = 1) {
	new Task:t = task_new();
	task_detach();

	inline const QueryFinished() {
		new insert_id = -1;
		if(cache_affected_rows()) {
			new
				stats[ItemBackpackStats],
				query[150];
			insert_id = cache_insert_id();
			mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_backpack(id, model, capacity) VALUES (%d, %d, %d)", insert_id, model, capacity);
			mysql_pquery(MySQL_Connection(), query);

			stats[iBpModel] = model;
			stats[iBpCapacity] = capacity;
			if(!map_valid(m_backpackStats)) {
				m_backpackStats = map_new();
			}
			map_set_arr(m_backpackStats, insert_id, stats);
			CallLocalFunction("Item_OnCreated", "iifi", insert_id, iAbilityBackpack, reliability, occupied_space);
		}
		task_set_result(t, insert_id);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO item_stats (ability, reliability, occupied_space) VALUES (%d, %.4f, %d)", iAbilityBackpack, reliability, occupied_space);
	task_set_result_ms(t, -1, 2000);
	return t;
}

stock IBackpack_GetCapacity(id) {
	if(!IBackpack_Has(id)) {
		return 0;
	}
	new stats[ItemBackpackStats];
	map_get_arr(m_backpackStats, id, stats);
	return stats[iBpCapacity];
}

stock IBackpack_GetFreeCapacity(id) {
	if(!IBackpack_Has(id)) {
		return 0;
	}

	new
		stats[ItemBackpackStats];
	map_get_arr(m_backpackStats, id, stats);
	if(IGroup_Has(id)) {
		new Iter:it = IGroup_GetIter(id);
		if(iter_valid(it)) {
			for(; iter_inside(it); iter_move_next(it)) {
				stats[iBpCapacity] -= Item_GetOccupiedSpace(iter_get(it));
			}
			iter_release(it);
		}
	}
	return stats[iBpCapacity];
}

stock IBackpack_GetModel(id) {
	if(!IBackpack_Has(id)) {
		return 0;
	}
	new
		stats[ItemBackpackStats];
	map_get_arr(m_backpackStats, id, stats);
	return stats[iBpModel];
}

forward IBackpack_OnDeleted(id);
public IBackpack_OnDeleted(id) {
	if(IBackpack_Has(id)) {
		map_remove(m_backpackStats, id);
		if(!map_size(m_backpackStats)) {
			map_delete(m_backpackStats);
			m_backpackStats = Map:-1;
		}
	}
	return 1;
}

hook OnGameModeInit@4() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_backpack (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	model MEDIUMINT DEFAULT 0,\
	capacity TINYINT UNSIGNED DEFAULT 0,\
	FOREIGN KEY (id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new query[150];
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM item_stats WHERE NOT EXISTS ( SELECT * FROM item_backpack WHERE item_stats.id = item_backpack.id ) AND item_stats.ability = %d", iAbilityBackpack);
	new Cache:result = mysql_query(MySQL_Connection(), query, true);
	if(cache_affected_rows()) {
		printf("INFO: Deleted %d rows with non-existing referenced `id` to item_backpack in item_stats.", cache_affected_rows());
	}
	cache_delete(result);
	return 1;
}

hook OnGameModeExit@4() {
	if(map_valid(m_backpackStats)) {
		map_delete(m_backpackStats);
		m_backpackStats = Map:-1;
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

forward IBackpack_OnStatsRetrieving(id, Task:t);
public IBackpack_OnStatsRetrieving(id, Task:t) {
	if(cache_num_rows()) {
		new stats[ItemBackpackStats];
		cache_get_value_name_int(0, "model", stats[iBpModel]);
		cache_get_value_name_int(0, "capacity", stats[iBpCapacity]);
		if(!map_valid(m_backpackStats)) {
			m_backpackStats = map_new();
		}
		map_set_arr(m_backpackStats, id, stats);
	}
	if(task_valid(t)) {
		task_set_result(t, 1);
	}
	return 1;
}

hook Item_OnStatsRetrieved(id, Task:t) {
	if(Item_GetAbility(id) == iAbilityBackpack) {
		new query[128];
		mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT * FROM item_backpack WHERE id = %d LIMIT 1", id);
		mysql_pquery(MySQL_Connection(), query, "IBackpack_OnStatsRetrieving", "ii", id, _:t);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook Item_OnModelGetAttempt(id, &model) {
	if(Item_GetAbility(id) == iAbilityBackpack) {
		model = IBackpack_GetModel(id);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook Item_OnNameGetAttempt(id, name[], size) {
	if(Item_GetAbility(id) == iAbilityBackpack) {
		name[0] = EOS;
		strcat(name, "Backpack", size);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IGroup_OnAddingAttempt(group, member, &error) {
	if(IBackpack_Has(group)) {
		new capacity = Item_GetFreeCapacity(group);
		if(capacity < 0 || Item_GetOccupiedSpace(member) > capacity) {
			error = grErrorCapacityLack;
		} else {
			error = grErrorNone;
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	error = grErrorNotSupported;
	return 1;
}
