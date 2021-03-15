#if defined faction_divisions_included
	#endinput
#endif

#define faction_divisions_included

enum _:FactionRank {
	rankName[32]
}

static Map:m_factionRanks;

stock Faction_AddRank(id, const name[]) {
	if(!Faction_Valid(id)) {
		return 0;
	}

	new
		LinkedList:ll,
		query[192];
	if(!map_has_key(m_factionRanks, id)) {
		ll = linked_list_new();
		map_set(m_factionRanks, id, ll);
	} else {
		ll = LinkedList:map_get(m_factionRanks, id);
	}
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO faction_ranks(id, rank_order) VALUES (%d, %d) ON DUPLICATE KEY UPDATE id = VALUES(id), rank_order = VALUES(rank_order)", id, linked_list_size(ll));
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
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM faction_ranks WHERE id = %d AND rank_order = %d", id, order);
	mysql_pquery(MySQL_Connection(), query);
	iter_erase(it);
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
	return 1;
}