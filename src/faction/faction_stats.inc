#if defined faction_stats_included
	#endinput
#endif

#define faction_stats_included

enum _:FactionStats {
	facName[64],
	bool:facType,
}

static Map:m_factionStats;

stock Task:Faction_Create(bool:legal) {
	new Task:t = task_new();

	new query[43 + 11 - 2 + 1];
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO faction_stats(type) VALUES (%d)", _:legal);
	mysql_pquery(MySQL_Connection(), query, "FactionStats_OnCreated", "dd", _:t, _:legal);
	return t;
}

stock Faction_Valid(id) {
	return map_has_key(m_factionStats, id);
}

stock bool:Faction_IsLegal(id) {
	if(map_has_key(m_factionStats, id)) {
		new fac[FactionStats];
		map_get_arr(m_factionStats, id, fac);
		return fac[facType];
	}
	return false;
}

stock Faction_GetName(id, name[], size = sizeof(name)) {
	if(!map_has_key(m_factionStats, id)) {
		return 0;
	}
	new fac[FactionStats];
	map_get_arr(m_factionStats, id, fac);
	format(name, sizeof(name), fac[facName]);
	return 1;
}

stock Faction_SetName(id, const name[]) {
	if(!map_has_key(m_factionStats, id)) {
		return 0;
	}
	new
		fac[FactionStats],
		query[128];
	map_get_arr(m_factionStats, id, fac);
	fac[facName][0] = EOS;
	strcat(fac[facName], name, sizeof(fac[facName]));
	map_set_arr(m_factionStats, id, fac);
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE faction_stats SET name = '%e' WHERE id = %d LIMIT 1", name, id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Faction_Delete(id) {
	if(!map_has_key(m_factionStats, id)) {
		return 0;
	}
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM faction_stats WHERE id = %d LIMIT 1", id);
	mysql_pquery(MySQL_Connection(), query);
	map_remove(m_factionStats, id);
	CallLocalFunction("Faction_OnDeleted", "i", id);
	return 1;
}

stock Iter:Faction_GetIter(index = 0) {
	new Iter:it = map_iter(m_factionStats, index);
	if(iter_valid(it) && iter_inside(it)) {
		iter_acquire(it);
		return it;
	}
	return Iter:0;
}

stock Iter:Faction_GetIterByKey(key) {
	new Iter:it = map_iter_at(m_factionStats, key);
	if(iter_valid(it) && iter_inside(it)) {
		iter_acquire(it);
		return it;
	}
	return Iter:0;
}

stock Faction_Count() {
	return map_size(m_factionStats);
}

forward FactionStats_OnCreated(Task:t, legal);
public FactionStats_OnCreated(Task:t, legal) {
	if(cache_affected_rows()) {
		new fac[FactionStats];
		fac[facName][0] = EOS;
		fac[facType] = bool:legal;
		map_set_arr(m_factionStats, insert_id, fac);
		task_set_result(t, cache_insert_id());
		return 1;
	}
	
	task_set_result(t, -1);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_factionStats = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS faction_stats (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
	name VARCHAR(64) DEFAULT '',\
	type TINYINT UNSIGNED DEFAULT FALSE\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM faction_stats", true);
	new rows = cache_num_rows();
	for(new i = 0, fac[FactionStats], id; i != rows; i++) {
		cache_get_value_name_int(i, "id", id);
		cache_get_value_name(i, "name", fac[facName], 64);
		cache_get_value_name_int(i, "type", fac[facType]);
		map_set_arr(m_factionStats, id, fac);
	}
	cache_delete(result);
	return 1;
}

hook OnGameModeExit() {
	if(map_valid(m_factionStats)) {
		map_delete(m_factionStats);
		m_factionStats = Map:-1;
	}
	return 1;
}