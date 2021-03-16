#if defined faction_ranks_included
	#endinput
#endif

#define faction_ranks_included

static Map:m_factionRanks;

stock Faction_AddRank(id, const name[]) {
	if(!Faction_Valid(id)) {
		return 0;
	}

	new
		LinkedList:ll,
		query[256];
	if(!map_has_key(m_factionRanks, id)) {
		ll = linked_list_new();
		map_set(m_factionRanks, id, ll);
	} else {
		ll = LinkedList:map_get(m_factionRanks, id);
	}
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO faction_ranks(id, rank_order, rank_name) VALUES (%d, %d, '%e') ON DUPLICATE KEY UPDATE id = VALUES(id), rank_order = VALUES(rank_order), rank_name = VALUES(rank_name)", id, linked_list_size(ll), name);
	mysql_pquery(MySQL_Connection(), query);
	linked_list_add_str(ll, name);
	return 1;
}

stock Faction_RemoveRank(id, order) {
	if(!map_has_key(m_factionRanks, id)) {
		return 0;
	}

	new
		LinkedList:ll = LinkedList:map_get(m_factionRanks, id),
		Iter:it = linked_list_iter(ll, order);
	if(!iter_valid(it)) {
		return 0;
	}
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM faction_ranks WHERE id = %d AND rank_order = %d", id, order);
	mysql_pquery(MySQL_Connection(), query);
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE faction_ranks SET rank_order = rank_order - 1 WHERE id = %d AND rank_order > %d", id, order);
	mysql_pquery(MySQL_Connection(), query);
	iter_erase(it);
	return 1;
}

stock Iter:Faction_GetRankIter(index) {
	if(!map_has_key(m_factionRanks, id)) {
		return Iter:0;
	}
	new Iter:it = map_iter(m_factionRanks, index);
	if(iter_valid(it) && iter_inside(it)) {
		iter_acquire(it);
		return it;
	}
	return Iter:0;
}

stock Iter:Faction_GetRankIterByKey(key) {
	if(!map_has_key(m_factionRanks, key)) {
		return Iter:0;
	}
	new Iter:it = map_iter_at(m_factionRanks, key);
	if(iter_valid(it) && iter_inside(it)) {
		iter_acquire(it);
		return it;
	}
	return Iter:0;
}

stock Faction_CountRanks(id) {
	if(map_has_key(m_factionRanks, id)) {
		return linked_list_size(LinkedList:map_get(m_factionRanks, id));
	}
	return 0;
}

stock Faction_SetRankName(id, order, const name[]) {
	if(!Faction_Valid(id) || !map_has_key(m_factionRanks, id)) {
		return 0;
	}
	new LinkedList:ll = LinkedList:map_get(m_factionRanks, id);
	if(order >= linked_list_size(ll)) {
		return 0;
	}

	new query[150];
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE faction_ranks SET rank_name = '%e' WHERE id = %d AND rank_order = %d LIMIT 1", name, id, order);
	mysql_pquery(MySQL_Connection(), query);
	linked_list_set_str(ll, order, name);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_factionRanks = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS faction_ranks (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED,\
	rank_order TINYINT UNSIGNED,\
	rank_name VARCHAR(32) DEFAULT '',\
	FOREIGN KEY (id) REFERENCES faction_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT COUNT(*) indexExists FROM INFORMATION_SCHEMA.STATISTICS WHERE table_schema=DATABASE() AND table_name='faction_ranks' AND index_name='unique_rank'", true),
		count;
	cache_get_value_name_int(0, "indexExists", count);
	if(!count) {
		mysql_query(MySQL_Connection(), "CREATE UNIQUE INDEX unique_rank ON faction_ranks(id, rank_order)", false);
	}
	cache_delete(result);

	result = mysql_query(MySQL_Connection(), "SELECT * FROM faction_ranks ORDER BY rank_order ASC", true);
	new rows = cache_num_rows();
	for(new i = 0, name[32], id, LinkedList:ll; i != rows; i++) {
		cache_get_value_name_int(i, "id", id);
		if(!map_has_key(m_factionRanks, id)) {
			ll = linked_list_new();
			map_set(m_factionRanks, id, ll);
		} else {
			ll = LinkedList:map_get(m_factionRanks, id);
		}
		cache_get_value_name(i, "rank_name", name);
		linked_list_add_str(ll, name);
	}
	return 1;
}