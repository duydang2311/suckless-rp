#if defined faction_stats_included
	#endinput
#endif

#define faction_stats_included

enum FactionStats {
	facName[64],
	bool:facType,
}

static Map:m_factionStats;

stock Task:Faction_Create(bool:legal) {
	new Task:t = task_new();

	inline const QueryFinished() {
		new insert_id = -1;
		if(cache_affected_rows()) {
			new fac[FactionStats];
			fac[facName][0] = EOS;
			fac[facType] = legal;
			insert_id = cache_insert_id();
			map_set_arr(m_factionStats, insert_id, fac);
		}
		task_set_result(t, insert_id);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO faction_types(type) VALUES (%d)", _:legal);
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
	new fac[FactionStats];
	map_get_arr(m_factionStats, id, fac);
	format(fac[facName], sizeof(fac[facName]), name);
	map_set_arr(m_factionStats, id, fac);
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

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_factionStats = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS faction_stats (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
	name VARCHAR(64) DEFAULT '',\
	type TINYINT UNSIGNED DEFAULT FALSE\
	)", false);
	return 1;
}