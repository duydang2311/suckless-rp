#if defined item_backpack_included
	#endinput
#endif

#define item_backpack_included

enum _:ItemBackpackStats {
	iBpModel,
	iBpCapacity
}

static Map:m_backpackStats;

stock Task:IBackpack_Create(model, capacity, Float:reliability = 1.0, occupied_space = 1) {
	new Task:t = task_new();
	task_detach();

	inline const QueryFinished() {
		new
			insert_id = cache_insert_id(),
			stats[ItemBackpackStats],
			query[150];
		mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_backpack(id, model, capacity) VALUES (%d, %d, %d)", insert_id, model, capacity);
		mysql_pquery(MySQL_Connection(), query);

		stats[iBpModel] = model;
		stats[iBpCapacity] = capacity;
		map_set_arr(m_backpackStats, insert_id, stats);
		CallLocalFunction("Item_OnCreated", "iifi", insert_id, iAbilityBackpack, reliability, occupied_space);
		task_set_result(t, insert_id);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO item_stats (ability, reliability, occupied_space) VALUES (%d, %.4f, %d)", iAbilityBackpack, reliability, occupied_space);
	return t;
}

stock IBackpack_GetCapacity(id) {
	if(!map_has_key(m_backpackStats, id)) {
		return 0;
	}
	new stats[ItemBackpackStats];
	map_get_arr(m_backpackStats, id, stats);
	return stats[iBpCapacity];
}

stock IBackpack_GetFreeCapacity(id) {
	if(!map_has_key(m_backpackStats, id)) {
		return 0;
	}

	new
		stats[ItemBackpackStats];
	map_get_arr(m_backpackStats, id, stats);
	if(Item_HasGroup(id)) {
		new Iter:it = Item_GetGroupIter(id);
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
	if(!map_has_key(m_backpackStats, id)) {
		return 0;
	}
	new
		stats[ItemBackpackStats];
	map_get_arr(m_backpackStats, id, stats);
	return stats[iBpModel];
}

forward IBackpack_OnDeleted(id);
public IBackpack_OnDeleted(id) {
	if(map_has_key(m_backpackStats, id)) {
		map_remove(m_backpackStats, id);
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_backpackStats = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_backpack (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	model MEDIUMINT DEFAULT 0,\
	capacity TINYINT UNSIGNED DEFAULT 0,\
	FOREIGN KEY (id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM item_backpack", true);
	for(new i = 0, rows = cache_num_rows(), stats[ItemBackpackStats], id; i != rows; i++) {
		cache_get_value_name_int(i, "id", id);
		cache_get_value_name_int(i, "model", stats[iBpModel]);
		cache_get_value_name_int(i, "capacity", stats[iBpCapacity]);
		map_set_arr(m_backpackStats, id, stats);
	}
	cache_delete(result);
	return 1;
}