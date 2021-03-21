#if defined faction_divisions_included
	#endinput
#endif

#define faction_divisions_included

static Map:m_factionDivisions;

stock Faction_AddDivision(id, const name[]) {
	if(!Faction_Valid(id)) {
		return 0;
	}

	new
		LinkedList:ll,
		query[256];
	if(!map_has_key(m_factionDivisions, id)) {
		ll = linked_list_new();
		map_set(m_factionDivisions, id, ll);
	} else {
		ll = LinkedList:map_get(m_factionDivisions, id);
	}
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO faction_divisions(id, div_order, div_name) VALUES (%d, %d, '%e') ON DUPLICATE KEY UPDATE id = VALUES(id), div_order = VALUES(div_order), div_name = VALUES(div_name)", id, linked_list_size(ll), name);
	mysql_pquery(MySQL_Connection(), query);
	linked_list_add_str(ll, name);
	return 1;
}

stock Faction_RemoveDivision(id, order) {
	if(!map_has_key(m_factionDivisions, id)) {
		return 0;
	}

	new
		LinkedList:ll = LinkedList:map_get(m_factionDivisions, id),
		Iter:it = linked_list_iter(ll, order);
	if(!iter_valid(it)) {
		return 0;
	}
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM faction_divisions WHERE id = %d AND div_order = %d", id, order);
	mysql_pquery(MySQL_Connection(), query);
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE faction_divisions SET div_order = div_order - 1 WHERE id = %d AND div_order > %d", id, order);
	mysql_pquery(MySQL_Connection(), query);
	iter_erase(it);
	return 1;
}

stock Iter:Faction_GetDivisionIter(index) {
	if(!map_has_key(m_factionDivisions, id)) {
		return Iter:0;
	}
	new Iter:it = map_iter(m_factionDivisions, index);
	if(iter_valid(it) && iter_inside(it)) {
		iter_acquire(it);
		return it;
	}
	return Iter:0;
}

stock Iter:Faction_GetDivisionIterByKey(key) {
	if(!map_has_key(m_factionDivisions, key)) {
		return Iter:0;
	}
	new Iter:it = map_iter_at(m_factionDivisions, key);
	if(iter_valid(it) && iter_inside(it)) {
		iter_acquire(it);
		return it;
	}
	return Iter:0;
}

stock Faction_CountDivisions(id) {
	if(map_has_key(m_factionDivisions, id)) {
		return linked_list_size(LinkedList:map_get(m_factionDivisions, id));
	}
	return 0;
}

stock Faction_SetDivisionName(id, order, const name[]) {
	if(!Faction_Valid(id) || !map_has_key(m_factionDivisions, id)) {
		return 0;
	}
	new LinkedList:ll = LinkedList:map_get(m_factionDivisions, id);
	if(order >= linked_list_size(ll)) {
		return 0;
	}

	new query[150];
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE faction_divisions SET div_name = '%e' WHERE id = %d AND div_order = %d LIMIT 1", name, id, order);
	mysql_pquery(MySQL_Connection(), query);
	linked_list_set_str(ll, order, name);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_factionDivisions = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS faction_divisions (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED,\
	div_order TINYINT UNSIGNED,\
	div_name VARCHAR(32) DEFAULT '',\
	FOREIGN KEY (id) REFERENCES faction_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT COUNT(*) indexExists FROM INFORMATION_SCHEMA.STATISTICS WHERE table_schema=DATABASE() AND table_name='faction_divisions' AND index_name='unique_division'", true),
		count;
	cache_get_value_name_int(0, "indexExists", count);
	if(!count) {
		mysql_query(MySQL_Connection(), "CREATE UNIQUE INDEX unique_division ON faction_divisions(id, div_order)", false);
	}
	cache_delete(result);

	result = mysql_query(MySQL_Connection(), "SELECT * FROM faction_divisions ORDER BY div_order ASC", true);
	new rows = cache_num_rows();
	for(new i = 0, name[32], id, LinkedList:ll; i != rows; i++) {
		cache_get_value_name_int(i, "id", id);
		if(!map_has_key(m_factionDivisions, id)) {
			ll = linked_list_new();
			map_set(m_factionDivisions, id, ll);
		} else {
			ll = LinkedList:map_get(m_factionDivisions, id);
		}
		cache_get_value_name(i, "div_name", name);
		linked_list_add_str(ll, name);
	}
	return 1;
}

hook OnGameModeExit() {
	if(map_valid(m_factionDivisions)) {
		map_delete(m_factionDivisions);
		m_factionDivisions = Map:-1;
	}
	return 1;
}