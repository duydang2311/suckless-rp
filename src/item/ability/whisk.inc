#if defined item_whisk_included
	#endinput
#endif

#define item_whisk_included

static Map:m_whiskStats;

stock Task:IWhisk_Create(Float:reliability = 1.0, occupied_space = 2) {
	new Task:t = task_new();
	task_detach();

	inline const QueryFinished() {
		new id = -1;
		if(cache_affected_rows()) {
			new query[128];
			id = cache_insert_id();
			if(!map_valid(m_whiskStats)) {
				m_whiskStats = map_new();
			}
			map_set(m_whiskStats, id, 1);
			mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_whisk(id) VALUES (%d)", id);
			mysql_pquery(MySQL_Connection(), query);
			CallLocalFunction("Item_OnCreated", "iifi", id, iAbilityWhisk, reliability, occupied_space);
		}
		task_set_result(t, id);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO item_stats (ability, reliability, occupied_space) VALUES (%d, %.4f, %d)", _:iAbilityWhisk, reliability, occupied_space);
	task_set_result_ms(t, -1, 2000);
	return t;
}

stock IWhisk_Has(id) {
	return (map_valid(m_whiskStats) && map_has_key(m_whiskStats, id));
}

stock IWhisk_GetModel() {
	return 2901;
}

hook OnGameModeInit@5() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_whisk (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	FOREIGN KEY (id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new query[150];
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM item_stats WHERE NOT EXISTS ( SELECT * FROM item_whisk WHERE item_stats.id = item_whisk.id ) AND item_stats.ability = %d", iAbilityWhisk);
	new Cache:result = mysql_query(MySQL_Connection(), query, true);
	if(cache_affected_rows()) {
		printf("INFO: Deleted %d rows with non-existing referenced `id` to item_whisk in item_stats.", cache_affected_rows());
	}
	cache_delete(result);
	return 1;
}

#include <YSI_Coding\y_hooks>

forward IWhisk_OnStatsRetrieving(id, Task:t);
public IWhisk_OnStatsRetrieving(id, Task:t) {
	if(cache_num_rows()) {
		if(!map_valid(m_whiskStats)) {
			m_whiskStats = map_new();
		}
		map_set(m_whiskStats, id, 1);
	}
	if(task_valid(t)) {
		task_set_result(t, 1);
	}
	return 1;
}

hook Item_OnStatsRetrieved(id, Task:t) {
	if(Item_GetAbility(id) == iAbilityWhisk) {
		new query[128];
		mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT * FROM item_whisk WHERE id = %d LIMIT 1", id);
		mysql_pquery(MySQL_Connection(), query, "IWhisk_OnStatsRetrieving", "ii", id, _:t);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook Item_OnModelGetAttempt(id, &model) {
	if(Item_GetAbility(id) == iAbilityWhisk) {
		model = IWhisk_GetModel();
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook Item_OnNameGetAttempt(id, name[], size) {
	if(Item_GetAbility(id) == iAbilityWhisk) {
		name[0] = EOS;
		strcat(name, "Whisk", size);
	}
	return 1;
}